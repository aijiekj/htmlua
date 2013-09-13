html
====

Lua's C module for parsing html text, wrapper of liigo::HtmlParser

LUA��HTML�����⣬Cģ�飬��װ��C++�Ŀ�[liigo::HtmlParser](https://github.com/liigo/html-parser)��

###������������

��HTML��������HTML�ı�����Ϊһ������Ľڵ�(Node)�����ڵ��Ϊ��ͬ�����ͣ���¼�˲�ͬ����Ϣ��
�ڵ�������У���ʼ��ǩ����&lt;a href="..."&gt;����������ǩ����&lt;/a&gt;�����ı���ע�͵ȡ�
��ʼ��ǩ�ͽ�����ǩ����һ����ǩ���ƣ�tagname����������"a"������Ӧ����һ����ǩ���ͣ�tagtype����������ʶ��ǩ����
���ڱ�Ҫ������£��Űѱ�ǩ����ʶ��Ϊ��ǩ���ͣ���ҪĿ������߽����ٶȡ���һ�������û��ɿصġ�
��ʼ��ǩ�ڵ�������һϵ�����ԣ�Attributes����ǰ���е�"href"����Ĭ������£�������ʶ�����ǩ���͵Ľڵ�Ž��������ԡ�

���ĵ��Ľڽ�ר�Ž��ܽڵ����ͺͱ�ǩ���͡�������Ϣ��ο�[��CSDN�����ϵ�����](http://blog.csdn.net/liigo/article/details/6153829)��

###һ������html�⣬����parser����������

	local html = require "html"

��html���󣨿���������������������������

	html.newparser(fn1,fn2)   -- ������������������������ʡ�ԣ�������ĵ���ڣ�������parser
	html.deleteparser(parser) -- ɾ�����������󣬲���Ϊparser���޷���ֵ

��html������һ���Զ������õ� parser ��Ա���ɹ�ֱ��ʹ�ã�������ʽ������ɾ����

###����parser���������󣬽���HTML�ı�

parser�����������·����������ȴ���parser������

	parser:parse(html) -- ����HTML�ı���������HTML�ı����޷���ֵ
	parser:nodecount() -- ���ؽ�����Ľڵ����
	parser:node(index) -- ���ؽ������ָ���������Ľڵ���󣬲����ǽڵ�����(>=1,<=nodecount())������ֵ��node���󣨿���Ϊnil��
	parser:ipairs()    -- ����֧��forѭ��˳������ڵ㣬�磺for index,node in parser:ipairs() do ...

������ parser:ipairs() ������������ for ѭ��˳����� node �ڵ����

	parser:parse("<html><body bg=red id=liigo>xxx<p x=123>...")
	for index,node in parser:ipairs() do
		print("node:", index, node.tagname, node.text)
	end

###����node�ڵ���󣬻�ȡ�ڵ���Ϣ

node���������³�Ա��

	node.type      -- �ڵ����ͣ�int��
	node.text      -- �ڵ��ı���string��
	node.tagname   -- ��ǩ���ƣ�string��
	node.tagtype   -- ��ǩ���ͣ�int��
	node.attrcount -- ���Ը�����int��
	node.iscdata   -- �Ƿ�CDATA���飨bool��
	node.isselfclosing -- �Ƿ��Խ�����ǩ��bool��������<br/>Ϊ�Խ�����ǩ��

node���������·����������ȴ���parser������

	node:attr(index/name) -- ȡָ������ֵ�����������������(string)����������ֵ(string)�������������������(>=1,<=attrcount)������������(string)������ֵ(string)���������ָ�������Բ����ڣ���������nil��
	node:pairs()     -- ����֧��forѭ���������ԣ��� for name,value in node:pairs() do ...
	node:parseattr() -- �޲����޷���ֵ�������������node�����С������ǰ�Ѿ��������������ظ�������������Զ�������ȷ����ǩ���͵Ľڵ����ԡ�

������ node:pairs() ������������ for ѭ�������ڵ����ԣ�

	local node1 = parser:node(1)
	for name,value in node1:pairs() do
		print("attr:", name, value)
	end

ע�⣬����node�Ĺ������豣֤parser����ʼ����Ч����û�е���parser:parse()ִ����һ�ν�����

###�ġ��ڵ����ͺͱ�ǩ����

�ڵ�������һ����������ʾ���������£�

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

���ǵ�������ȾLUAȫ�ֱ�����û�а���������Cģ���С��û���Ҫ�ڳ��������ж���ڵ����ͳ�����ͨ���������ϴ��뼴�ɡ�ʹ�÷����� if(nodetype == htmlnode.START_TAG) ...

��ǩ����Ҳ��һ����������ʾ���������£�

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

���ǵ�������ȾLUAȫ�ֱ�����û�а���������Cģ���С����Ҷ��������û�б�Ҫʶ����ô���ǩ���ͣ��û��ɸ����������ȷ��Ҫʶ��ı�ǩ���ڳ��������ж���ڵ����ͳ�����ͨ���������ϴ��뼴�ɡ�ʹ�÷����� if(tagtype == htmltag.DIV) ...

TODO(liigo): �����ϳ������嵽Cģ���У��Է����û�ʹ��Ϊ��Ҫ�����㡣����LUA��table�ǹ�ϣ���ӱ�ǩ���Ʋ�ѯ��ǩ���ͣ������ϲ�Ӱ������ٶȡ���Ӧ�ػ��ڿ���Ҫ��Ҫɾ���ص�fnIdentifyHtmlTag�����ӻص�onParseAttributes��

###�塢parser�������Ļص�����

���� html.newparser() ��������ʡ�ԵĲ������ɽ�������������Ϊ�ص������������ڽ��������б����ã���ԭ�����£�

	function fnIdentifyHtmlTag (tagname, nodetype) number
	function fnOnNodeReady (node) bool

�ص����� fnIdentifyHtmlTag �ڽ�������ʼ��ǩ�ͽ�����ǩʱ�����ã�����ʶ��ñ�ǩ���ƶ�Ӧ�ı�ǩ���͡��������������ֱ��Ǳ�ǩ����tagname���ı����ͽڵ�����nodetype��������������ֵ�Ǳ�ǩ����tagtype����������ʶ��ı�ǩԽ�٣������ٶ�Խ�졣���ڽ����ٶȵĿ��ǣ����Ǳ�Ҫ������ʶ���ǩ���͡�ֻ��ȷ���˱�ǩ���͵ı�ǩ�������ԲŻᱻ�Զ�����������ֻ���ֹ���������node:parseattr()����

�ص����� fnOnNodeReady �ڽ������ÿ���ڵ�󱻵��ã��û����ڴ�ȷ���Ƿ�������������Ĳ����ǽڵ����node������ֵ��bool������true��ʾ��������������false��ʾ��ֹ������

����Ĳ������� test.lua ����ʹ���������ص�������ʾ���������������£�

	local htmltag = {
		UNKNOWN = 0, SCRIPT=1, STYLE=2, TEXTAREA=3, 
		A=101, DIV=... IMG=... ��������
	}

	local function identifyHtmlTag(tagname, nodetype)
		--print("identifyHtmlTag:", tagname, nodetype)
		return htmltag[string.upper(tagname)] or htmltag.UNKNOWN
	end

	local function onNodeReady(node)
		--print("onnodeready:", node.tagname, node.text)
		return (node.tagname ~= "break")
	end

	local parser = html.newparser(identifyHtmlTag, onNodeReady)
