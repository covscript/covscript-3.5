import minparser_visitor as ast_visitor
import minparser as syntax
import parsergen

var parser = new parsergen.generator
parser.add_grammar("ecs-lang", syntax.grammar)
parser.from_file("./minimal.csc")
var visitor = new ast_visitor.main
visitor.run(system.out, parser.ast)
var ops = visitor.get_op_sequence()
foreach i in range(ops.size)
    system.out.println(to_string(i) + ":\t" + ops[i])
end
