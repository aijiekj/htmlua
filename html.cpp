#include "html.h"
#include "HtmlParser.h"
#include <stdlib.h>

extern "C"{
	#include <lua.h>
	#include<lauxlib.h>
}


class LuaHtmlParser : public liigo::HtmlParser {
private:
	lua_State* L;
	int refOnIdentifyHtmlTag;
public:
	LuaHtmlParser(lua_State* L_, int refOnIdentifyHtmlTag_) { this->L = L_; refOnIdentifyHtmlTag = refOnIdentifyHtmlTag_; }
	virtual ~LuaHtmlParser() {}
	int get_refOnIdentifyHtmlTag() { return refOnIdentifyHtmlTag; }

public:
	//�������า��, �Ա�ʶ������ǩ(��߽�������), ����ʶ����ٱ�ǩ(��߽����ٶ�)
	//Ĭ�Ͻ�ʶ���漰HTML�����ṹ����Ϣ�����޼�����ʼ��ǩ: A,IMG,META,BODY,TITLE,FRAME,IFRAME
	//onIdentifyHtmlTag()����onParseAttributes()������
	virtual liigo::HtmlTagType onIdentifyHtmlTag(const char* szTagName, liigo::HtmlNodeType nodeType) {
		if(refOnIdentifyHtmlTag != LUA_NOREF && refOnIdentifyHtmlTag != LUA_REFNIL) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, refOnIdentifyHtmlTag);
			if(lua_isfunction(L, -1)) {
				lua_pushstring(L, szTagName);
				lua_pushinteger(L, (int)nodeType);
				lua_call(L, 2, 1);
				return (liigo::HtmlTagType)lua_tointeger(L, -1);
			}
		}
		return HtmlParser::onIdentifyHtmlTag(szTagName, nodeType);
	}

	//�������า��, �Ա���õĽ����ڵ�����, ���߲��ֽ��������ɴ಻�����ڵ�����(��߽����ٶ�)
	//���Ը��ݱ�ǩ����(pNode->tagName)���ǩ����(pNode->tagType)�ж��Ƿ���Ҫ�������ԣ�parseAttributes()��
	//Ĭ�Ͻ�����"��ʶ�����ǩ����"�ı�ǩ���ԣ���pNode->tagType != TAG_UNKNOWN��
	virtual void onParseAttributes(liigo::HtmlNode* pNode) {
		HtmlParser::onParseAttributes(pNode);
	}

	//�������า��, ��ĳ�ڵ������ɺ󱻵���, �������false������ֹͣ����HTML
	virtual bool onNodeReady(liigo::HtmlNode* pNode) { return true; }

};


struct LuaFunc {
	const char* name;
	int (*func) (lua_State*);
};

// create_lua_funcs_table() push a new created table on stack top, which contains all functions.
// if tname is no NULL, will save the table to Registry, to be take back later by using get_lua_funcs_table(L,tname).
// it is recommended to use a prefix for tname to avoid name confusion with other names in global Registry.
// by liigo, 20130906.
static void create_lua_funcs_table(lua_State* L, LuaFunc* funcs, const char* tname) {
	lua_newtable(L);
	int t = lua_gettop(L);
	while(funcs && funcs->name && funcs->func) {
		lua_pushcfunction(L, funcs->func);
		lua_setfield(L, t, funcs->name);
		funcs++;
	}
	if(tname) {
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_REGISTRYINDEX, tname);
	}
}

// push the table on stack top
// returns nil if no prev create_lua_funcs_table() called with the same name.
static void get_lua_funcs_table(lua_State* L, const char* tname) {
	lua_getfield(L, LUA_REGISTRYINDEX, tname);
}

static void lua_print(lua_State* L, const char* msg) {
	lua_getglobal(L, "print");
	lua_pushstring(L, msg);
	lua_call(L, 1, 0);
}

static void report_lua_error(lua_State* L, const char* errmsg) {
	lua_pushstring(L, errmsg);
	lua_error(L);
}

// get parser from self parameter. ensure returned parser not NULL.
static liigo::HtmlParser* getparser(lua_State* L) {
	if(!lua_istable(L,1))
		report_lua_error(L, "require parser value as first parameter");
	lua_rawgeti(L, 1, 0); //@see html_newparser(). FIXME: use named key to check type? (integer key has most efficiency)
	liigo::HtmlParser* parser = (liigo::HtmlParser*) lua_touserdata(L, -1);
	lua_pop(L, 1);
	if(parser == NULL)
		report_lua_error(L, "invalid parser value");
	return parser;
}

// parse html text
// arg: parser (self), html (string), parseAttributes (bool)
// no returns, use parser:node(index) to get the parsed nodes.
static int parser_parse(lua_State* L) {
	const char* html = lua_tostring(L, 2);
	int parseAttributes = lua_toboolean(L, 3);
	liigo::HtmlParser* parser = getparser(L);
	parser->parseHtml(html, parseAttributes == 1);
	return 0;
}

static int parser_node(lua_State* L);

// iterator function for parser's nodes, see parser_ipairs()
// returns: index+1, node
// arg: parser, index
static int parser_nextnode(lua_State* L) {
	lua_pushinteger(L, lua_tointeger(L, -1) + 1);
	lua_replace(L, -2); // change index to index+1
	parser_node(L);
	if(lua_isnil(L, -1)) { // if node is nil
		lua_pushnil(L);
		lua_replace(L, -3); // change index to nil, to break for loop
	}
	return 2;
}

// ipairs support for loop for parser's nodes: 
//   for index,node in parser:ipairs() do ...
// returns: a next() function, parser, 0
// arg: parser
static int parser_ipairs(lua_State* L) {
	if(lua_gettop(L) != 1 || !lua_istable(L, -1)) {
		report_lua_error(L, "ipairs() requires a parser as the only parameter");
	}
	lua_pushcclosure(L, parser_nextnode, 0); // lua5.2: lua_pushcfuntion(L, parser_nextnode);
	lua_insert(L, -2);
	lua_pushinteger(L, 0);
	return 3;
}

// return node count
static int parser_nodecount(lua_State* L) {
	liigo::HtmlParser* parser = getparser(L);
	lua_pushinteger(L, parser->getHtmlNodeCount());
	return 1;
}

// nodeindex: 1 based
inline static bool isvalid_nodeindex(liigo::HtmlParser* parser, int nodeindex) {
	return (nodeindex >= 1 && nodeindex <= parser->getHtmlNodeCount());
}

// return HtmlNode* or NULL. nodeindex is 1 based
static liigo::HtmlNode* getparsernode(liigo::HtmlParser* parser, int nodeindex) {
	if(isvalid_nodeindex(parser, nodeindex)) {
		return parser->getHtmlNode(nodeindex - 1); // 0 based
	}
	return NULL;
}

// get node from self parameter. ensure returned node not NULL.
static liigo::HtmlNode* getnodeself(lua_State*L) {
	if(!lua_istable(L,1))
		report_lua_error(L, "require node value as first parameter");
	lua_rawgeti(L, 1, 0); //@see parser_node()
	liigo::HtmlNode* node = (liigo::HtmlNode*) lua_touserdata(L, -1);
	lua_pop(L, 1);
	if(node == NULL)
		report_lua_error(L, "invalid node value");
	return node;
}

static int node_attr(lua_State* L);

// iterator function for node's attributes, see node_pairs()
// returns: attrname, attrvalue
// arg: node, (ignor)
static int node_nextattr(lua_State* L) {
	lua_pop(L, 1);
	lua_rawgeti(L, -1, 1); // read iterate index at node[1]
	int index = lua_tointeger(L, -1);
	lua_pop(L, 1);
	lua_pushinteger(L, index + 1);
	lua_rawseti(L, -2, 1); // node[1] = index + 1
	lua_pushinteger(L, index + 1);
	node_attr(L); //return attr(index+1)
	return 2;
}

// ipairs support for loop for node's attributes: 
//   for index,node in parser:ipairs() do ...
// returns: a next() function, parser, nil
// arg: node
static int node_pairs(lua_State* L) {
	if(lua_gettop(L) != 1 || !lua_istable(L, -1)) {
		report_lua_error(L, "pairs() requires a node as the only parameter");
	}
	lua_pushinteger(L, 0);
	lua_rawseti(L, -2, 1); //node[1] = 0, store the iterate index
	lua_pushcclosure(L, node_nextattr, 0); // lua5.2: lua_pushcfuntion(L, parser_nextnode);
	lua_insert(L, -2);
	lua_pushnil(L);
	return 3;
}

// return node's attribute:
//   value (string), if arg 2 is attrname (string);
//   name (string) and value (string), if arg 2 is attrindex (integer);
//   nil and nil if no such attribute;
// arg: node (self), attrname (string) or attrindex (integer)
static int node_attr(lua_State* L) {
	liigo::HtmlNode* node = getnodeself(L);

	const liigo::HtmlAttribute* attr = NULL;
	if(lua_isnumber(L, 2)) {
		int attrindex = lua_tointeger(L, 2) - 1;
		if(attrindex >= 0 && attrindex < node->attributeCount) {
			attr = liigo::HtmlParser::getAttribute(node, attrindex);
			if(attr) {
				lua_pushstring(L, attr->name);
				lua_pushstring(L, attr->value ? attr->value : "");
			} else {
				lua_pushnil(L);
				lua_pushnil(L);
			}
			return 2;
		}
	} else if(lua_isstring(L, 2)) {
		const char* attrname = lua_tostring(L, 2);
		attr = liigo::HtmlParser::getAttribute(node, attrname);
		if(attr)
			lua_pushstring(L, attr->value ? attr->value : "");
		else
			lua_pushnil(L);
		return 1;
	}

	lua_pushnil(L);
	lua_pushnil(L);
	return 2;
}

// parse attributes from node.text, and store result to node.
// arg: node (self)
// no returns, no repeat parsing if the node attributes has been parsed before
static int node_parseattr(lua_State* L) {
	liigo::HtmlNode* node = getnodeself(L);
	liigo::HtmlParser::parseAttributes(node);
	return 0;
}

// return HtmlNode* or NULL. nodeindex is 1 based
// arg: parse, nodeindex
static liigo::HtmlNode* getnode(lua_State* L) {
	liigo::HtmlParser* parser = getparser(L);
	int nodeindex = lua_tointeger(L, 2);
	return getparsernode(parser, nodeindex);
}

// returns node (table) at nodeindex, or nil if no such node
// arg: parser (self), nodeindex (int)
// node's property:
//   type
//   text
//   tagtype
//   tagname
//   attrcount
//   iscdata
//   isselfclosing
// node's method
//   attr("name")
//   pairs(void)
//   parseattr(void)
static int parser_node(lua_State* L) {
	liigo::HtmlNode* node = getnode(L);
	if(node == NULL) {
		lua_pushnil(L);
		return 1;
	}

	LuaFunc funcs[] = {
		{ "attr", node_attr },
		{ "pairs", node_pairs },
		{ "parseattr", node_parseattr },
		{ NULL, NULL }
	};

	create_lua_funcs_table(L, funcs, NULL); // the node table to be return

	lua_pushinteger(L, node->type);
	lua_setfield(L, -2, "type");

	lua_pushstring(L, node->text ? node->text : "");
	lua_setfield(L, -2, "text");

	lua_pushinteger(L, node->tagType);
	lua_setfield(L, -2, "tagtype");

	lua_pushstring(L, node->tagName);
	lua_setfield(L, -2, "tagname");

	lua_pushinteger(L, node->attributeCount);
	lua_setfield(L, -2, "attrcount");

	lua_pushboolean(L, node->flags & liigo::FLAG_SELF_CLOSING_TAG);
	lua_setfield(L, -2, "isselfclosing");

	lua_pushboolean(L, node->flags & liigo::FLAG_CDATA_BLOCK);
	lua_setfield(L, -2, "iscdata");

	lua_pushlightuserdata(L, node);
	lua_rawseti(L, -2, 0); // self[0] = node

	return 1;
}


// returns a new 'parser' (table value)
// arg: optional function 'onIdentifyHtmlTag'
//   tagtype onIdentifyHtmlTag(tagname, nodetype)
//   int onIdentifyHtmlTag(string, int)
static int html_newparser(lua_State* L) {
	LuaFunc funcs[] = {
		{ "parse", parser_parse },
		{ "ipairs", parser_ipairs },
		{ "node",  parser_node },
		{ "nodecount", parser_nodecount },
		{ NULL, NULL }
	};

	int refOnIdentifyHtmlTag = LUA_NOREF;
	if(lua_gettop(L) > 0 && (lua_isfunction(L,-1) || lua_isstring(L,-1))) {
		refOnIdentifyHtmlTag = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	liigo::HtmlParser* parser = new LuaHtmlParser(L, refOnIdentifyHtmlTag);
	create_lua_funcs_table(L, funcs, NULL);
	lua_pushlightuserdata(L, parser);
	lua_rawseti(L, -2, 0); // self[0] = parser; @see getparser()
	return 1;
}

// arg: parser
static int html_deleteparser(lua_State* L) {
	LuaHtmlParser* parser = (LuaHtmlParser*) getparser(L);
	if(parser) {
		luaL_unref(L, LUA_REGISTRYINDEX, parser->get_refOnIdentifyHtmlTag());
		delete parser;
		lua_pushnil(L);
		lua_rawseti(L, -2, 0); // self[0] = NULL
	}
	return 0;
}

// arg: html table
static int html__gc(lua_State* L) {
	lua_getfield(L, -1, "parser");
	html_deleteparser(L);
	return 0;
}

extern "C"
int luaopen_html(lua_State* L) {
	LuaFunc funcs[] = {
		{ "newparser", html_newparser },
		{ "deleteparser", html_deleteparser },
		{ NULL, NULL }
	};
	LuaFunc metafuncs[] = {
		{ "__gc", html__gc },
		{ NULL, NULL }
	};

	create_lua_funcs_table(L, funcs, NULL); // the table 'html' to return
	int html = lua_gettop(L);
	create_lua_funcs_table(L, metafuncs, NULL); // the metatable
	lua_setmetatable(L, -2); // html.metatable = metatable

	// auto create a parser, ready to use
	lua_pushnil(L);
	html_newparser(L);
	lua_setfield(L, html, "parser"); // html.parser = new parser
	lua_settop(L, html);

	return 1;
}
