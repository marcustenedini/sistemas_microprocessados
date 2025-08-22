import math

def simplificar(a: int,b: int) -> tuple[int,int]:
    mdc= math.gcd(a,b)
    return a //mdc, b // mdc

qtd_testes=input()
for i in range(0,qtd_testes):
    test_temp=input().split()
    print(test_temp)
    num1,den1=int(test_temp[0]),int(test_temp[2])
    oper,num2,den2=test_temp[3],int(test_temp[4]),int(test_temp[6])
    mmc= lambda x,y: x*y
    sum= lambda x,y: x+y
    sub= lambda x,y: x-y
    transf= lambda numerador,denominador,mmc: numerador*(mmc//denominador)
    mult= lambda x,y:x*y
    if oper == '+':
        denominador=mmc(den1,den2) 
        num1,num2=transf(num1,den1,denominador),transf(num2,den2,denominador)
        numerador=sum(num1,num2)
        new_n,new_d=simplificar(numerador,denominador)
    elif oper== '-':
        denominador=mmc(den1,den2)
        num1,num2=transf(num1,den1,denominador),transf(num2,den2,denominador)
        numerador=sub(num1,num2)
        new_n,new_d=simplificar(numerador,denominador)
    elif oper== '*':
        numerador= mult(num1,num2)
        denominador=mult(den1,den2)
        new_n,new_d=simplificar(numerador,denominador)
    elif oper=='/':  
        numerador= mult(num1,den2)
        denominador=mult(den1,num2)
        new_n,new_d=simplificar(numerador,denominador)
    else:
        print('error')         

    print(f'{numerador} / {denominador} = {new_n} / {new_d}')
