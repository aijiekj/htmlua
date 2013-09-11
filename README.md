html
====

Lua's C module for parsing html text, wrapper of liigo::HtmlParser

LUA��HTML�����⣬Cģ�飬��װ��C++�Ŀ�[liigo::HtmlParser](https://github.com/liigo/html-parser)��

###������������

��HTML��������HTML�ı�����Ϊһ������Ľڵ�(Node)�����ڵ��Ϊ��ͬ�����ͣ���¼�˲�ͬ����Ϣ��
�ڵ�������У���ʼ��ǩ����&lt;a href="..."&gt;����������ǩ����&lt;/a&gt;�����ı���ע�͵ȡ�
��ʼ��ǩ�ͽ�����ǩ����һ����ǩ���ƣ�tagname,��������"a"������Ӧ����һ����ǩ���ͣ�tagtype,��������ʶ��ǩ����
���ڱ�Ҫ������£��Űѱ�ǩ����ʶ��Ϊ��ǩ���ͣ���ҪĿ������߽����ٶȡ���һ�������û��ɿصġ�
��ʼ��ǩ�ڵ�������һϵ�����ԣ�Attributes,��ǰ���е�"href"����Ĭ������£�������ʶ�����ǩ���͵Ľڵ�Ž��������ԡ�

������Ϣ��ο�[��CSDN�����ϵĽ�������](http://blog.csdn.net/liigo/article/details/6153829)��

###һ������html�⣬����parser����������

	local html = require "html"

��html���󣨿���������������������������

	html.newparser(fnIdentifyHtmlTag) -- �������������󣬲�����ʡ��(������)������parser
	html.deleteparser(parser)         -- ɾ�����������󣬲���Ϊparser���޷���ֵ

��html������һ���Զ������õ� parser ��Ա���ɹ�ֱ��ʹ�ã�������ʽ������ɾ����

html.newparser�Ŀ�ѡ�����ĺ���ԭ���� int fnIdentifyHtmlTag(string, int)��
�ú����Ĳ����Ǳ�ǩ����tagname���ı����ͽڵ�����nodetype��������������ֵ��tagtype����������
�ú����Ĺ����ǣ����ݴ���ı�ǩ����ȷ����ǩ���Ͳ����ء�
ʶ��ı�ǩԽ�٣������ٶ�Խ�졣ֻ��ȷ���˱�ǩ���͵ı�ǩ�������ԲŻᱻ�Զ�����������ֻ���ֹ���������node:parseattr()����
���ڽ����ٶȵĿ��ǣ����Ǳ�Ҫ������ʶ���ǩ���͡�
����Ĳ������� test.lua ����ʹ�øú�����ʾ���������������£�

	local htmltag = {
		UNKNOWN = 0, SCRIPT=1, STYLE=2, TEXTAREA=3, 
		A=101, DIV=... IMG=... ��������
	}

	local function identifyHtmlTag(tagname, nodetype)
		return htmltag[string.upper(tagname)] or htmltag.UNKNOWN
	end

	local parser = html.newparser(identifyHtmlTag)

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
