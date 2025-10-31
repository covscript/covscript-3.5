var a = input()
var b = 1*2 + 3
var c = {1, {2, 3}, 4}
if a > 0
    print("Hello")
else
    print("Wrong")
end
loop
    b = b - 1
    print(b)
    b += 2
    if b > 0
        var c = "Ohh"
        print(add(c, "Ohh"))
    end
    if b == 0
        break
    else
        continue
    end
end
exit(0)