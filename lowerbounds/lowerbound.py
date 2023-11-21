from  math import floor, pow, log, ceil

# Formulas from Lemma 5
# https://doi.org/10.1093/jrsssb/qkad067

def f(d, k):
	if d<0 or k<0: 
		raise Exception(f"Positive values expected. Found d={d} k={k}")
	if type(k)!=int:
		raise Exception(f"k should be an integer: type({k})={type(k)}")		
	if not k or not d: return 0
	#if d==1: return k+1 # alternative
	if type(k)!=int: raise Exception(d,k)
	return bplus(d,k)+f(d-1,k-b(d,k))


def fx(d,k):
	return f(d,k),bplus(d,k),f(d-1,k-b(d,k)),k-b(d,k)
	
def k1dfloorpowfix(d,k):   # also fixed!
	p = int(floor(pow(k,1/d)))	
	if (p+1)**d==k: return p+1 # exact 
	return p


def b(d,k):
	k1d=k1dfloorpowfix(d,k)
	ldk=l(d,k)	
	return k1d**(d-ldk)*(k1d+1)**ldk

def bplus(d,k):
	if type(k)!=int: raise Exception(d,k)
	k1d=k1dfloorpowfix(d,k)
	ldk=l(d,k)	
	return (k1d+1)**(d-ldk)*(k1d+2)**ldk

def l(d,k):
	k1d=k1dfloorpowfix(d,k)	
	if k==k1d**d: return 0 # fix 

	a=log(k)-d*log(k1d)
	b=log(k1d+1)-log(k1d)	
	return floor(a/b)

def rk(k,d):
	return min( ((f(d,j)-j)/f(d,j)) for j in range(1,k+1))

def minargrk(k,d):
	mrk = rk(k,d)
	return [j  for j in range(1,k+1) if ((f(d,j)-j)/f(d,j))==mrk ] 


def cube_sk(k,c1,c2,c3):
	rkv = rk(k,3)
	V=c1*c2*c3
	Vplus=(c1+1)*(c2+1)*(c3+1)	
	return rkv*Vplus-(Vplus-V)

def lowerboundsk(k,c1,c2,c3):
	return ceil(cube_sk(k,c1,c2,c3))


"""
A simple test for some exemplary data
"""
def testing():

	fl=open("rk.csv","w")
	fl.write("k,d,rk,f,b,bplus,k1d,ldk\n")
	for d in [2,3,4]:
		for n in [1,2,3,4,5]:
			k=n**d
			k1d=k1dfloorpowfix(k,d)
			vf, bp, fp, km, bv = f(d,k),bplus(d,k),f(d-1,k-b(d,k)),k-b(d,k),b(d,k)
			print(f"n={n} d={d} k={k} f({d},{k})={f(d,k)}",end=' ')
			print(f"b({d},{k})={b(d,k)}",end=' ')
			print(f"bplus({d},{k})={bplus(d,k)}",end=' ')
			print(f"f({d-1},{k-bv})={f(d-1,k-bv)}",end=' ')				
			print(f"k1d={k1d}",end=' ')				
			rkv=rk(k,d)			
			print(f"rk({k})={rkv},l(d,k)={l(d,k)}  mrk={minargrk(k,d)}",end=' ')				
			fl.write(f"{k},{d},{rkv},{vf},{bv},{bp},{k1d},{l(d,k)}\n")
			print()

	fl.close()




