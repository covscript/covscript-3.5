import minparser_visitor as ast_visitor
import minparser as syntax
import parsergen

var parser = new parsergen.generator
parser.add_grammar("ecs-lang", syntax.grammar)
parser.from_file("./minimal.csc")
(new ast_visitor.main).run(system.out, parser.ast)
