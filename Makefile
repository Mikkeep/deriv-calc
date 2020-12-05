Options = -Wall -Wpedantic

# (sin (      5)) + ( ( x  ) * (10     )   )    
# (((x) - (1)) ^ (3)) * (((x) - (2)) ^ (-2))
# (sin(((pi) * ((n) + (1))) / (2))) * ((e) ^ (x))    
# ((x) - (20)) * (((9) * (x)) + (1))
# ((((3)*(tan(x)))+(cos(sin(exp(x)))))*((10)-((2)^(log(x)))))/((x)^(2))
# (sin((3) * ((pi) / (2)))) * (cos(((pi) * (x)) ^ (e)))

SrcDir = src
BinDir = bin
IntDir = $(BinDir)/intermediates
LibDir = libs

LIBS = $(wildcard $(LibDir)/*.a)
DEPS = $(wildcard $(SrcDir)/*.h) $(wildcard $(LibDir)/*.h)
OBJS = $(IntDir)/main.o $(IntDir)/math_syntax.o $(IntDir)/expression_tree.o $(IntDir)/expression_loader.o $(IntDir)/expression_simplifier.o $(IntDir)/differentiation.o $(IntDir)/taylor_expansion.o

$(BinDir)/deriv_calc.exe: $(OBJS) $(LIBS) $(DEPS)
	g++ -o $(BinDir)/deriv_calc.exe $(OBJS) $(LIBS)

$(IntDir)/main.o: $(SrcDir)/main.cpp $(DEPS)
	g++ -o $(IntDir)/main.o -c $(SrcDir)/main.cpp $(Options)

$(IntDir)/math_syntax.o: $(SrcDir)/math_syntax.cpp $(DEPS)
	g++ -o $(IntDir)/math_syntax.o -c $(SrcDir)/math_syntax.cpp $(Options)

$(IntDir)/expression_tree.o: $(SrcDir)/expression_tree.cpp $(DEPS)
	g++ -o $(IntDir)/expression_tree.o -c $(SrcDir)/expression_tree.cpp $(Options)

$(IntDir)/expression_loader.o: $(SrcDir)/expression_loader.cpp $(DEPS)
	g++ -o $(IntDir)/expression_loader.o -c $(SrcDir)/expression_loader.cpp $(Options)

$(IntDir)/expression_simplifier.o: $(SrcDir)/expression_simplifier.cpp $(DEPS)
	g++ -o $(IntDir)/expression_simplifier.o -c $(SrcDir)/expression_simplifier.cpp $(Options)

$(IntDir)/differentiation.o: $(SrcDir)/differentiation.cpp $(DEPS)
	g++ -o $(IntDir)/differentiation.o -c $(SrcDir)/differentiation.cpp $(Options)

$(IntDir)/taylor_expansion.o: $(SrcDir)/taylor_expansion.cpp $(DEPS)
	g++ -o $(IntDir)/taylor_expansion.o -c $(SrcDir)/taylor_expansion.cpp $(Options)