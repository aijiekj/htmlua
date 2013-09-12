local html = require "html"

local parser = html.newparser()
parser:parse("<html><body bg=red id=liigo>xxx<p x=123>...", true)

for index,node in parser:ipairs() do
	print(index, node.tagname, node.text)
end

local node = parser:node(2)
print(node.tagname, node.text, node.attrcount)
print(node:attr("bg"), node:attr("id"), node:attr("nothisattr"))

html.deleteparser(parser)

--����ڵ�(Node)����
local htmlnode = {
	NULL      = 0, --��Ϊ���нڵ���ս��ǣ�LUA��δ�õ���
	START_TAG = 1, --��ʼ��ǩ���� <a href="liigo.com"> �� <br/>
	END_TAG   = 2, --������ǩ���� </a>
	CONTENT   = 3, --����: ���ڿ�ʼ��ǩ��/�������ǩ֮�����ͨ�ı�
	REMARKS   = 4, --ע��: <!-- -->
	UNKNOWN   = 5, --δ֪�Ľڵ�����
	_USER_ = 10, --�û�����������ڵ�����ֵӦ����_USER_����ȷ���������涨��ĳ���ֵ�ظ�
}

--�����ǩ(Tag)����
local htmltag = {
	UNKNOWN = 0, --��ʾδ��ʶ��ı�ǩ���ͣ��μ�HtmlParser.onIdentifyHtmlTag()
	SCRIPT=1, STYLE=2, TEXTAREA=3, --���ڽ�����Ҫ����ʶ��<script>,<style>��<textarea>���ڲ��ر���
	--���°���ǩ��ĸ˳������, ��Դ��http://www.w3.org/TR/html4/index/elements.html (HTML4)
	--���� http://www.w3.org/TR/html5/section-index.html#elements-1 (HTML5)
	A=101, ABBR=102, ACRONYM=103, ADDRESS=104, APPLET=105, AREA=106, ARTICLE=107, ASIDE=108, AUDIO=109,
	B=110, BASE=111, BASEFONT=112, BDI=113, BDO=114, BIG=115, BLOCKQUOTE=116, BODY=117, BR=118, BUTTON=119,
	CAPTION=120, CENTER=121, CITE=122, CODE=123, COL=124, COLGROUP=125, COMMAND=126,
	DATALIST=127, DD=128, DEL=129, DETAILS=130, DFN=131, DIR=132, DIV=133, DL=134, DT=135, EM=136, EMBED=137,
	FIELDSET=138, FIGCAPTION=139, FIGURE=140, FONT=141, FOOTER=142, FORM=143, FRAME=144, FRAMESET=145,
	H1=146, H2=147, H3=148, H4=149, H5=150, H6=151, HEAD=152, HEADER=153, HGROUP=154, HR=155, HTML=156,
	I=157, IFRAME=158, IMG=159, INPUT=160, INS=161, ISINDEX=162, KBD=163, KEYGEN=164,
	LABEL=165, LEGEND=166, LI=167, LINK=168, MAP=169, MARK=170, MENU=171, META=172, METER=173, NAV=174, NOFRAMES=175, NOSCRIPT=176,
	OBJECT=177, OL=178, OPTGROUP=179, OPTION=180, P=181, PARAM=182, PRE=183, PROGRESS=184, Q=185, RP=186, RT=187, RUBY=188,
	S=189, SAMP=190, SECTION=191, SELECT=192, SMALL=193, SOURCE=194, SPAN=195, STRIKE=196, STRONG=197, SUB=198, SUMMARY=199, SUP=200,
	TABLE=201, TBODY=202, TD=203, TFOOT=204, TH=205, THEAD=206, TIME=207, TITLE=208, TR=209, TRACK=210, TT=211,
	U=212, UL=213, VAR=214, VIDEO=215, WBR=216,
	_USER_=300, --�û������������ǩ����ֵӦ����_USER_����ȷ���������涨��ĳ���ֵ�ظ�
}

local function identifyHtmlTag(tagname, nodetype)
	--print("identifyHtmlTag:", tagname, nodetype)
	return htmltag[string.upper(tagname)] or htmltag.UNKNOWN
end

local function onNodeReady(node)
	print("onnodeready:", node.tagname, node.text)
	return (node.tagname ~= "break")
end

local function testNodes()
	print("------Test Nodes------")
	local parser = html.newparser(identifyHtmlTag) --identify html tags
	parser:parse("<body>000<a>111</a><BR/>")
	assert(parser:nodecount() == 6)
	for index,node in parser:ipairs() do
		print("node:", index, node.tagname, node.text)
	end

	local node1 = parser:node(1)
	assert(node1.type == htmlnode.START_TAG)
	assert(node1.tagname == "body");
	assert(node1.tagtype == htmltag.BODY)
	assert(node1.text == "")
	assert(node1.attrcount == 0)

	assert(parser:node(2).text == "000")
	assert(parser:node(0) == nil and parser:node(10) == nil) -- no such node

	html.deleteparser(parser)
end

local function testNodeAttributes()
	print("------Test attributes------")
	local parser = html.newparser()
	parser:parse("<a href='liigo.com' color=red checked>111</a>", true) -- parameter parseAttributes: true
	assert(parser:nodecount() == 3)
	local node1 = parser:node(1)
	assert(node1 ~= nil)
	assert(node1.attrcount == 3) --count of attributes
	for name,value in node1:pairs() do
		print("attr: ", name, value)
	end
	--get attribute value by name
	assert(node1:attr("href") == "liigo.com")
	assert(node1:attr("color") == "red")
	assert(node1:attr("checked") == "")
	assert(node1:attr("nosuchattr") == nil)
	--get attribute name and value by index
	local n1,v1 = node1:attr(1)
	assert(n1 == "href" and v1 == "liigo.com")
	local n2,v2 = node1:attr(2)
	assert(n2 == "color" and v2 == "red")
	local n3,v3 = node1:attr(3)
	assert(n3 == "checked" and v3 == "") --value is "" not nil
	local n4,v4 = node1:attr(4)
	assert(n4 == nil and v4 == nil) --no such attribute
	html.deleteparser(parser)
end

local function testParserCallbacks()
	print("------Test Callbacks------")
	local parser = html.newparser(identifyHtmlTag,onNodeReady)
	parser:parse("<1><2><3>...<5 x=0><break><6><7>")
	html.deleteparser(parser)
end

testNodes()
testNodeAttributes()
testParserCallbacks()

print("test html over")
