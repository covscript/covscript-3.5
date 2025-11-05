import minparser_visitor as ast_visitor
import minparser as syntax
import parsergen

var parser = new parsergen.generator
parser.add_grammar("ecs-lang", syntax.grammar)
parser.from_file("./minimal.csc")
var visitor = new ast_visitor.main
visitor.run(system.out, parser.ast)
var str_seg = visitor.get_str_segment()
foreach i in range(str_seg.size)
    system.out.println(to_string(i) + ":\t" + str_seg[i])
end
var code_seg = visitor.get_code_segment()
foreach i in range(code_seg.size)
    system.out.println(to_string(i) + ":\t" + code_seg[i])
end
