# coding:UTF8
import string

ald = {'б':'6','п':'n', 'д':'g', 'а': 'a', 'с': 'c', 'і': 'i', 'о': 'o', 'р': 'p', 'к': 'k', 'х': 'x', 'г': 'r', 'у': 'y', 'е': 'e', 'А': 'A', 'Х': 'X', 'М': 'M', 'Н': 'H', 'Т': 'T', 'Е': 'E', 'І': 'I', 'О': 'O', 'Р': 'P', 'К': 'K', 'С': 'C', 'В': 'B', 'З':'3'}
count = 0
sof = set()
def get(element):
    global count
    count += 1
    return ald.get(element,False)

def sad(element):
    global sof
    if element != " ":
        sof.add(element)
    return element

line = input(">>>")
line = [get(i) if ald.get(i,False) else sad(i) for i in line]
line = "".join(line)
print(count)
print(len(sof))
if len(sof )> 8:
    print("maybe this is bad idea")
print("\"" + " " * (((20 - len(line))//2)+count) + line + "\"")
print("\"" + " " * (((20 - len(line))//2)) + line + "\"")
