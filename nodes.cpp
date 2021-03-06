/*
	Nodes for TCSS
*/

//Definitions
class Node;
class Nodes;
class RulesetNode;

//Variables
std::map<std::string, std::string> vars;

//Declarations
std::map<std::string, std::string> decls;
 
//The Node Class
class Node
{
	public:
	virtual std::string evaluate() = 0;
	void print(std::string s, bool nl = true)
	{
		std::cout << s;
		if(nl)
		{
			std::cout << "\n";
		}
	}
};

//List of Nodes (Statements)
class Nodes : public std::list<Node *>
{
	public:
	//Evaluate All Nodes in a List
	std::string evaluate()
	{
		std::string css = "";
		Nodes::iterator it;
		for(it=this->begin(); it != this->end(); ++it)
		{
			css += (*it)->evaluate();
		}
		
		return css;
	}
};

/*
	Node Types
*/

//The Entire Stylesheet
class StyleNode : public Node
{
	public:
	Node *charset;
	Node *import;
	Nodes *body;
	
	StyleNode(Node *c, Node *i, Nodes *b)
	{
		this->charset = c;
		this->import = i;
		this->body = b;
	}
	
	std::string evaluate()
	{
		//CSS
		std::string css;
		if(charset)
			css = charset->evaluate();
		
		if(import)
			css = import->evaluate();
		
		if(this->body)
			css = body->evaluate();
		return css;
	}
};

class CharsetNode : public Node
{
	public:
	std::string charset;
	CharsetNode(std::string charset)
	{
		this->charset = charset;
	}
	
	std::string evaluate()
	{
		return "@charset \"" + charset + "\"";
	}
};

class ImportNode : public Node
{
	public:
	std::string import;
	ImportNode(std::string import = NULL)
	{
		this->import = import;
	}
	
	std::string evaluate()
	{
		return "@import \"" + import + "\"";
	}
};


/*
	=============================================
	START:	If Condition Nodes
	=============================================
*/

/*
	If Node
	Example:
		@if Windows{
			p{
				font-family:"Times New Roman";
			}
		}
*/
class IfNode : public Node
{
	public:
	Node *cond;
	Nodes *body;
	IfNode(Node *c, Nodes *b)
	{
		this->cond = c;
		this->body = b;
	}
	
	std::string evaluate()
	{
		std::string veracity = cond->evaluate();
		if(veracity == "true")
		{
			return body->evaluate();
		}
		
		else
		{
			return "";
		}
	}
};

/*
	Conditional Identifier Node
	Example:
		Windows|Linux|Mac|UnknownOS
		Firefox|IE|Safari|Chrome|Opera|[etc]
*/
class CondIdNode : public Node
{
	public:
	std::string ident;
	CondIdNode(std::string id)
	{
		this->ident = id;
	}
	
	std::string evaluate()
	{
		if(agent_idents.find(ident) != agent_idents.end())
		{
			return agent_idents[ident] ? "true" : "false";
		}
		
		else
		{
			return "false";
		}
	}
};

/*
	Conditional Comparison Node
	Example:
		Version > 8.0
		OS = Windows
		Browser = Firefox
*/
class CondCmpNode : public Node
{
	public:
	std::string ident;
	std::string op;
	Node *exp;
	CondCmpNode(std::string id, std::string o, Node *e)
	{
		this->ident = id;
		this->op = o;
		this->exp = e;
	}
	
	std::string evaluate()
	{
		//The only string that is supported is
		//currently "Version" for browser versions.
		
		//CondIdNode *cid = new CondIdNode(ident);
		//std::string veracity = cid->evaluate();
		int version = agent_idents["Version"];
		std::string eval_exp = exp->evaluate();
		int versionOperand = atoi(eval_exp.c_str());
		
		if(op == "=")
			return version == versionOperand ? "true" : "false";
		else if(op == ">")
			return version > versionOperand ? "true" : "false";
		else if(op == "<")
			return version < versionOperand ? "true" : "false";
		else if(op == ">=")
			return version >= versionOperand ? "true" : "false";
		else if(op == "<=")
			return version <= versionOperand ? "true" : "false";
		else
			return "false";
	}
};

/*
	Conditional And Node
	Example:
		IE and Version > 8.0
*/
class CondAndNode : public Node
{
	public:
	Node *left;
	Node *right;
	CondAndNode(Node *l, Node *r)
	{
		this->left = l;
		this->right = r;
	}
	
	std::string evaluate()
	{
		//Veracity of Conditions
		std::string vleft = left->evaluate();
		std::string vright = right->evaluate();
		if(vleft == "true" && vright == "true")
		{
			return "true";
		}
		
		else
		{
			return "false";
		}
	}
};

/*
	Conditional Or Node
	Example:
		Opera or Safar or Chrome
*/
class CondOrNode : public Node
{
	public:
	Node *left;
	Node *right;
	CondOrNode(Node *l, Node *r)
	{
		this->left = l;
		this->right = r;
	}
	
	std::string evaluate()
	{
		//Veracity of Conditions
		std::string vleft = left->evaluate();
		std::string vright = right->evaluate();
		if(vleft == "true" || vright == "true")
		{
			return "true";
		}
		
		else
		{
			return "false";
		}
	}
};

/*
	Conditional Not Node
	Example:
		!Windows
*/
class CondNotNode : public Node
{
	public:
	Node *right;
	CondNotNode(Node *r)
	{
		this->right = r;
	}
	
	std::string evaluate()
	{
		//Veracity of Condition
		std::string vright = right->evaluate();
		if(vright == "true")
		{
			return "false";
		}
		
		else
		{
			return "true";
		}
	}
};

/*
	=============================================
	END:	If Condition Nodes
	=============================================
*/



/*
	=============================================
	START:	Media Query Nodes
	=============================================
*/

//Media Query Node
class MediaQueryNode : public Node
{
	public:
	Nodes *list, *rulesets;
	MediaQueryNode(Nodes *l, Nodes *r)
	{
		this->list = l;
		this->rulesets = r;
	}
	
	std::string evaluate()
	{
		return "@media " + list->evaluate() + "{\n" + rulesets->evaluate() + "}\n";
	}
};

/*
	Query Node
	only|not	media_expr_list
	empty		media_expr_list
*/
class QueryNode : public Node
{
	public:
	std::string notOnly;
	Nodes *list;
	QueryNode(std::string n, Nodes *l)
	{
		this->notOnly = n;
		this->list = l;
	}
	
	std::string evaluate()
	{
		return notOnly + " " + list->evaluate();
	}
};


/*
	Media Featire Node
	(width = 50px)
	(screen)
*/
class MediaFeatureNode : public Node
{
	public:
	std::string feature;
	std::string op;
	Node *expr;
	MediaFeatureNode(std::string f, std::string o = "", Node *e = NULL)
	{
		this->feature = f;
		this->op = o;
		this->expr = e;
	}
	
	std::string evaluate()
	{
		//(media_feature)
		if(op == "")
		{
			return "(" + this->feature + ")";
		}
		
		else
		{
			/*
				Media Feature Operators
				Equality
					=	->	:
					>=	->	: min-
					<=	->	: max-
			*/
			if(op == "=" || op == ":")
			{
				return "(" + this->feature + ": " + this->expr->evaluate() + ")";
			}
			
			else if(op == ">=")
			{
				return "(min-" + this->feature + ": " + this->expr->evaluate(); ")";
			}
			
			else if(op == "<=")
			{
				return "(max-" + this->feature + ": " + this->expr->evaluate(); ")";
			}
		}
	}
};

/*
	=============================================
	END:	Media Query Nodes
	=============================================
*/


//Operator Node
class OpNode : public Node
{
	public:
	Node *left, *right;
	char op;
	OpNode(Node *left, char op, Node *right)
	{
		this->left = left;
		this->op = op;
		this->right = right;
	}
	
	std::string evaluate()
	{
		return "calc(" + left->evaluate() + " " + op + " " + right->evaluate() + ")";
	}
};

/*
	Declaration Node
	Example:
		font:arial serif 30px !important;
*/
class DeclNode : public Node
{
	public:
	std::string prop;
	Nodes *value;
	bool important;
	
	DeclNode(std::string p, Nodes *v, bool i=false)
	{
		this->prop = p;
		this->value = v;
		this->important = i;
	}
	
	std::string evaluate()
	{
		return "\t" + prop + ": " + value->evaluate()  + ((important) ? "!important" : "") + ";\n";
	}
};

//String Node
class StrNode : public Node
{
	public:
	std::string str;
	StrNode(std::string s)
	{
		this->str = s;
	}
	
	std::string evaluate()
	{
		return str;
	}
};

//Identifier Node
class IdNode : public Node
{
	public:
	std::string id;
	IdNode(std::string i)
	{
		this->id = i;
	}
	
	std::string evaluate()
	{
		return id;
	}
};

/*
	Function Node
	Example:
		nth-of-child(even)
*/
class FuncNode : public Node
{
	public:
	std::string name;
	Node *exp;
	FuncNode(std::string n, Node *e)
	{
		this->name = n;
		this->exp = e;
	}
	
	std::string evaluate()
	{
		return name + ((exp) ? exp->evaluate() : "") + ")";
	}
};

/*
	Page Node
	Example:
		@page [:first] { color:red; }
*/
class PageNode : public Node
{
	public:
	std::string type;
	Nodes *declarations;
	
	PageNode(std::string t, Nodes *d)
	{
		this->type = t;
		this->declarations = d;
	}
	
	std::string evaluate()
	{
		return "@page" + ((type != "") ? " :" + type : "") + "{\n" + declarations->evaluate() + "}\n\n";
	}
};

/*
	Variable Declaration Node
	Example:
		$Vars{ mainColor: red; }
*/
class VarDeclNode : public Node
{
	public:
	std::string className;
	Nodes *declarations;
	
	VarDeclNode(std::string c, Nodes *d = NULL)
	{
		this->className = c;
		this->declarations = d;
	}
	
	std::string evaluate()
	{
		//Process
		decls[className] = declarations->evaluate();
		return "";
	}
};

/*
	Variable Declaration Include Node
	Example:
		.big-button{ $button; }
*/
class VarDeclIncNode : public Node
{
	public:
	std::string className;
	
	VarDeclIncNode(std::string c)
	{
		this->className = c;
	}
	
	std::string evaluate()
	{
		//Process
		return decls[className];
	}
};

std::string recursive_ruleset(RulesetNode *ruleset, std::string selector = "");

/*
	Ruleset Node
	Example:
		.css { color:red; }
*/
class RulesetNode : public Node
{
	public:
	Nodes *selector_list;
	Nodes *declarations;
	Nodes *sub_rulesets;
	
	RulesetNode(Nodes *s, Nodes *d = NULL, Nodes *sub = NULL)
	{
		this->selector_list = s;
		this->declarations = d;
		this->sub_rulesets = sub;
	}
	
	std::string evaluate()
	{	
		return recursive_ruleset(this);
	}
};

//Recursive Subruleset Function
std::string recursive_ruleset(RulesetNode *ruleset, std::string selector)
{
	//Result String
	std::string result = "";
	
	//Current Selector Name
	std::string list = ruleset->selector_list->evaluate();
	std::string selector_string = selector + (isalpha(list[0]) ? " " : "") + list;
	
	//Get Declarations
	result += selector_string + "{\n" + ((ruleset->declarations) ? ruleset->declarations->evaluate() : "") + "}\n\n";
	
	//Get Subrulesets
	if(!ruleset->sub_rulesets)
	{
		return result;
	}
	
	for(Nodes::iterator it = ruleset->sub_rulesets->begin(); it != ruleset->sub_rulesets->end(); ++it)
	{
		result += recursive_ruleset((RulesetNode*) *it, selector_string);
	}
	
	return result;
}

/*
	Pseudo Selector Node
	Example:
		a:hover | a:nth-of-type(even)
*/
class PseudoSelectorNode : public Node
{
	public:
	Node *selector;
	Node *pseudo;
	
	PseudoSelectorNode(Node *s, Node *p)
	{
		this->selector = s;
		this->pseudo = p;
	}
	
	std::string evaluate()
	{
		return selector->evaluate() + pseudo->evaluate();
	}
};

/*
	Pseudo Block Node
	Example:
		:hover | :nth-of-type(even)
*/
class PseudoBlockNode : public Node
{
	public:
	std::string ident;
	Node *function = NULL;
	
	PseudoBlockNode(std::string i)
	{
		this->ident = i;
	}
	
	PseudoBlockNode(Node *f)
	{
		this->function = f;
	}
	
	std::string evaluate()
	{
		return (ident != "") ? ":" + ident : ":" + function->evaluate();
	}
};

/*
	Pseudo Element Node
	Example:
		::selection
*/
class PseudoElementNode : public Node
{
	public:
	std::string name;
	
	PseudoElementNode(std::string n)
	{
		this->name = n;
	}
	
	std::string evaluate()
	{
		return "::" + name;
	}
};


/*
	Separator Node
	Used for printing out commas and spaces in lists of items.
	Example
		html, body | html body
*/
class SeparatorNode : public Node
{
	public:
	std::string sep;
	
	SeparatorNode(std::string s)
	{
		this->sep = s;
	}
	
	std::string evaluate()
	{
		return sep;
	}
};


/*
	Selector Node
	Example:
		body | .class | #id
*/
class SelectorNode : public Node
{
	public:
	std::string name;
	
	SelectorNode(std::string n)
	{
		this->name = n;
	}
	
	std::string evaluate()
	{
		return name;
	}
};

/*
	Combinator Node
	Example:
		h1 + h2 | h1 > h2 | h1 ~ h2
*/
class CombinatorNode : public Node
{
	public:
	Node *left;
	char op;
	Node *right;
	
	CombinatorNode(Node *l, char o, Node *r)
	{
		this->left = l;
		this->right = r;
		this->op = o;
	}
	
	std::string evaluate()
	{
		return left->evaluate() + " " + op + " " + right->evaluate();
	}
};

/*
	Attribute Selector Node
	Example:
		[tcss] | [tcss=value]
*/
class AttrSelectNode : public Node
{
	public:
	std::string ident;
	Node *op;
	Node *value;
	
	AttrSelectNode(std::string i, Node *o = NULL, Node *v = NULL)
	{
		this->ident = i;
		this->op = o;
		this->value = v;
	}
	
	std::string evaluate()
	{
		return "[" + ident + ((value) ? op->evaluate() + value->evaluate() : "") + "]";
	}
};

//URI Node
class UriNode : public Node
{
	public:
	std::string uri;
	UriNode(std::string u)
	{
		this->uri = u;
	}
	
	std::string evaluate()
	{
		return uri;
	}
};

//Number Node
class NumNode : public Node
{
	public:
	std::string num;
	NumNode(std::string n)
	{
		this->num = n;
	}
	
	std::string evaluate()
	{
		return num;
	}
};

//Dimension Node
class DimNode : public Node
{
	public:
	std::string dim;
	DimNode(std::string d)
	{
		this->dim = d;
	}
	
	std::string evaluate()
	{
		return dim;
	}
};

/*
	Hash Node
	Example:
		#33aaff | #3af
*/
class HashNode : public Node
{
	public:
	std::string hash;
	HashNode(std::string h)
	{
		this->hash = h;
	}
	
	std::string evaluate()
	{
		return hash;
	}
};

/*
	Var Node
	Example:
		$ident
*/
class VarNode : public Node
{
	public:
	std::string var;
	VarNode(std::string v)
	{
		this->var = v;
	}
	
	std::string evaluate()
	{
		//Not Found
		if(vars.find(var) ==  vars.end())
		{
			TCSS::warning("Use of undefined variable " + var);
			return "";
		}
		
		else
		{
			return vars[var];
		}
	}
};

/*
	Var Assign Node
	Example:
		$ident : exp;
*/
class VarAssignNode : public Node
{
	public:
	std::string var;
	Nodes *exp;
	VarAssignNode(std::string v, Nodes *e = NULL)
	{
		this->var = v;
		this->exp = e;
	}
	
	std::string evaluate()
	{
		std::string val = "";
		if(exp)
			val = exp->evaluate();
		vars[var] = val;
		
		return "";
	}
};