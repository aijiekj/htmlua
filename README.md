htmlua
======

Lua's C module for parsing html text, wrapper of liigo::HtmlParser

LUA��HTML�����⣬Cģ�飬��װ��C++�Ŀ� [liigo::HtmlParser](https://github.com/liigo/html-parser)��

###������������

��HTML��������HTML�ı�����Ϊһ������Ľڵ�(Node)�����ڵ��Ϊ��ͬ�����ͣ���¼�˲�ͬ����Ϣ��
�ڵ�������У���ʼ��ǩ����&lt;a href="..."&gt;����������ǩ����&lt;/a&gt;�����ı���ע�͵ȡ�
��ʼ��ǩ�ͽ�����ǩ����һ����ǩ���ƣ�tagname����������"a"������Ӧ����һ����ǩ���ͣ�tagtype����������ʶ��ǩ����
��ʼ��ǩ�ڵ�������һϵ�����ԣ�Attributes����ǰ���е�"href"����

���ĵ��Ľڽ�ר�Ž��ܽڵ����ͺͱ�ǩ���͡�������Ϣ��ο�[��CSDN�����ϵ�����](http://blog.csdn.net/liigo/article/details/6153829)��

###һ������htmlua�⣬����parser����������

	local html = require "htmlua"

��html���󣨿���������������������������

	html.newparser(fn1,fn2)   -- ������������������������ʡ�ԣ�������ĵ���ڣ�������parser
	html.deleteparser(parser) -- ɾ�����������󣬲���Ϊparser���޷���ֵ

��html������һ���Զ������õ� parser ��Ա���ɹ�ֱ��ʹ�ã�������ʽ������ɾ����

###����parser���������󣬽���HTML�ı�

parser�����������·����������ȴ���parser������Ϊ��һ����������

	parser:parse(html,[parseAttr]) -- ����HTML�ı�������1��HTML�ı�������2ָ���Ƿ�����ڵ����ԣ�Ĭ��Ϊtrue�����޷���ֵ
	parser:nodecount() -- ���ؽ�����Ľڵ����
	parser:node(index) -- ���ؽ������ָ���������Ľڵ���󣬲����ǽڵ�����(>=1,<=nodecount())������ֵ��node���������Ƿ�ʱ����nil��
	parser:ipairs()    -- ����֧��forѭ��˳������ڵ㣬�磺for index,node in parser:ipairs() do ...

������ parser:ipairs() ������������ for ѭ��˳����� node �ڵ����

	parser:parse("<html><body bg=red id=liigo>xxx<p x=123>...")
	for index,node in parser:ipairs() do
		print("node:", index, node.tagname, node.text)
	end

###����node�ڵ���󣬻�ȡ�ڵ���Ϣ

node���������³�Ա��

	node.type      -- �ڵ����ͣ�int������Ϊ htmlnode.START_TAG, htmlnode.END_TAG... �ȳ���ֵ֮һ��������Ľ�htmlnode
	node.text      -- �ڵ��ı���string��
	node.tagname   -- ��ǩ���ƣ�string��
	node.tagtype   -- ��ǩ���ͣ�int������Ϊ htmltag.A, htmltag.DIV, htmltag.IMG... �ȳ���ֵ֮һ��������Ľ�htmltag
	node.attrcount -- ���Ը�����int��
	node.iscdata   -- �Ƿ�CDATA���飨bool��
	node.isselfclosing -- �Ƿ��Խ�����ǩ��bool��������<br/>Ϊ�Խ�����ǩ��

node���������·����������ȴ���node������Ϊ��һ����������

	node:attr(index/name) -- ȡָ������ֵ�����������������(string)����������ֵ(string)�������������������(>=1,<=attrcount)������������(string)������ֵ(string)���������ָ�������Բ����ڣ���������nil��
	node:pairs()     -- ����֧��forѭ���������ԣ��� for name,value in node:pairs() do ...
	node:parseattr() -- �����ڵ����ԣ��޲����޷���ֵ�������������node�����С������ǰ�Ѿ��������������ظ�������

������ node:pairs() ������������ for ѭ�������ڵ����ԣ�

	local node1 = parser:node(1)
	for name,value in node1:pairs() do
		print("attr:", name, value)
	end

ע�⣬����node�Ĺ������豣֤parser����ʼ����Ч����û�е���parser:parse()ִ����һ�ν�����

###�ġ��ڵ����ͺͱ�ǩ����

�ڵ�������һ����������ʾ�����������ȶ��������½ڵ����ͳ�����

	htmlnode = {
		START_TAG = 1, --��ʼ��ǩ���� <a href="liigo.com"> �� <br/>
		END_TAG   = 2, --������ǩ���� </a>
		CONTENT   = 3, --����: ���ڿ�ʼ��ǩ��/�������ǩ֮�����ͨ�ı�
		REMARKS   = 4, --ע��: <!-- -->
		UNKNOWN   = 5, --δ֪�Ľڵ�����
		_USER_    = 10, --�û�����������ڵ�����ֵӦ����_USER_����ȷ���������涨��ĳ���ֵ�ظ�
	}

ʹ�÷����� if(nodetype == htmlnode.START_TAG) ...

��ǩ����Ҳ��һ����������ʾ�����������ȶ��������±�ǩ���ͳ�����

	htmltag = {
		UNKNOWN = 0, --��ʾδ��ʶ��ı�ǩ���ͣ��μ�HtmlParser.onIdentifyHtmlTag()
		SCRIPT=1, STYLE=2, TEXTAREA=3, --���ڽ�����Ҫ����ʶ��<script>,<style>��<textarea>���ڲ��ر���
		--���°���ǩ��ĸ˳������, ��Դ��http://www.w3.org/TR/html4/index/elements.html (HTML4)
		--���� http://www.w3.org/TR/html5/section-index.html#elements-1 (HTML5)
		A=11, ABBR=12, ACRONYM=13, ADDRESS=14, APPLET=15, AREA=16, ARTICLE=17, ASIDE=18, AUDIO=19,
		B=20, BASE=21, BASEFONT=22, BDI=23, BDO=24, BIG=25, BLOCKQUOTE=26, BODY=27, BR=28, BUTTON=29,
		CAPTION=30, CENTER=31, CITE=32, CODE=33, COL=34, COLGROUP=35, COMMAND=36,
		DATALIST=37, DD=38, DEL=39, DETAILS=40, DFN=41, DIR=42, DIV=43, DL=44, DT=45, EM=46, EMBED=47,
		FIELDSET=48, FIGCAPTION=49, FIGURE=50, FONT=51, FOOTER=52, FORM=53, FRAME=54, FRAMESET=55,
		H1=56, H2=57, H3=58, H4=59, H5=60, H6=61, HEAD=62, HEADER=63, HGROUP=64, HR=65, HTML=66,
		I=67, IFRAME=68, IMG=69, INPUT=70, INS=71, ISINDEX=72, KBD=73, KEYGEN=74,
		LABEL=75, LEGEND=76, LI=77, LINK=78, MAP=79, MARK=80, MENU=81, META=82, METER=83, NAV=84, NOFRAMES=85, NOSCRIPT=86,
		OBJECT=87, OL=88, OPTGROUP=89, OPTION=90, P=91, PARAM=92, PRE=93, PROGRESS=94, Q=95, RP=96, RT=97, RUBY=98,
		S=99, SAMP=100, SECTION=101, SELECT=102, SMALL=103, SOURCE=104, SPAN=105, STRIKE=106, STRONG=107, SUB=108, SUMMARY=109, SUP=110,
		TABLE=111, TBODY=112, TD=113, TFOOT=114, TH=115, THEAD=116, TIME=117, TITLE=118, TR=119, TRACK=120, TT=121,
		U=122, UL=123, VAR=124, VIDEO=125, WBR=126,
		_USER_=150, --�û������������ǩ����ֵӦ����_USER_����ȷ���������涨��ĳ���ֵ�ظ�
	}

ʹ�÷����� if(tagtype == htmltag.DIV) ...

###�塢parser�������Ļص�����

���� html.newparser([fnOnParseAttr],[fnOnNodeReady]) ��������ʡ�ԵĲ������ɽ�������������Ϊ�ص������������ڽ��������б���ε��ã���ԭ�����£�

	function fnOnParseAttr(node) bool
	function fnOnNodeReady(node) bool

�ص����� fnOnParseAttr �ڽ�������ʼ��ǩ����Ҫ��������ʱ�����á������ǵ�ǰ�ڵ����node������ֵ������bool������true��ʾ��Ҫ�������ԣ�����false��ʾ����Ҫ������˲�����ʡ�Ի�Ϊnil���ȼ��ڷ���true��parser:parse()����parseAttrΪfalse������²�����ô˻ص�������

�ص����� fnOnNodeReady �ڽ������ÿ���ڵ�󱻵��ã��û����ڴ�ȷ���Ƿ�������������ڵ㡣�����ǵ�ǰ�ڵ����node������ֵ������bool������true��ʾ��������������false��ʾ��ֹ����������˲�����ʡ�Ի�Ϊnil���ȼ��ڷ���true��

����Ĳ������� test.lua ����ʹ���������ص�������ʾ���������������£�

	local function onParseAttr(node)
		print("onParseAttr:", node.tagname, node.text)
		return (node.tagname == "2")
	end

	local function onNodeReady(node)
		print("onNodeReady:", node.tagname, node.text)
		return (node.tagname ~= "break")
	end

	local parser = html.newparser(identifyHtmlTag, onNodeReady)
