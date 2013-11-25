def defined(name):
    if(name in globals()):
        return True
    if(name in locals()):
        return True
    if(name in vars()):
        return True
    names = name.split(".")
    if(len(names)>1):
        if(names[1] in dir(globals()[names[0]])):
            return True
    return False

def dlog(text, level=0):
    print(text)

regs = ["EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EBP", "ESP", "EIP"]

def decode_op1(dbg, op1):
    my_op = op1
    if(my_op[0] == '['):
        my_op = decode_op1(dbg, my_op[1:-1])
        my_op = int(struct.unpack("<i", "".join(dbg.read(my_op, 4)))[0]) & 0xffffffff
        return my_op
    for reg in regs:
        if(my_op.upper() == reg):
            my_op = dbg.get_register(reg)
            return my_op & 0xffffffff
    if(len(my_op.split("+")) >1):
        (a,b) = my_op.split("+")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a+b
        return my_op & 0xffffffff
    if(len(my_op.split("-")) >1):
        (a,b) = my_op.split("-")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a-b
        return my_op & 0xffffffff
    if(len(my_op.split("*")) >1):
        (a,b) = my_op.split("*")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a*b
        return my_op & 0xffffffff
    return int(my_op, 16) & 0xffffffff


