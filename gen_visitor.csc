import visitorgen, minparser
var ofs = iostream.ofstream("./ast_visitor.csp")
(new visitorgen.visitor_generator).run(ofs, minparser.grammar.stx)