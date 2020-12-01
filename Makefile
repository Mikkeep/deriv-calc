Options = -Wall -Wpedantic

# (sin (      5)) + ( ( x  ) * (10     )   )    
# (((x) - (1)) ^ (3)) * (((x) - (2)) ^ (-2))
# (sin(((pi) * ((n) + (1))) / (2))) * ((e) ^ (x))    
# ((x) - (20)) * (((9) * (x)) + (1))
# ((((3)*(tan(x)))+(cos(sin(exp(x)))))*((10)-((2)^(log(x)))))/((x)^(2))
# (sin((3) * ((pi) / (2)))) * (cos(((pi) * (x)) ^ (e)))

SrcDir = src
BinDir = bin
Intermediates = $(BinDir)/intermediates
LibDir = libs

LIBS = $(wildcard $(LibDir)/*.a)
DEPS = $(wildcard $(SrcDir)/*.h) $(wildcard $(LibDir)/*.h)
OBJS = $(Intermediates)/main.o $(Intermediates)/math_syntax.o $(Intermediates)/expression_tree.o $(Intermediates)/expression_loader.o $(Intermediates)/expression_simplifier.o $(Intermediates)/differentiation.o

$(BinDir)/deriv_calc.exe: $(OBJS) $(LIBS) $(DEPS)
	g++ -o $(BinDir)/deriv_calc.exe $(OBJS) $(LIBS)

$(Intermediates)/main.o: $(SrcDir)/main.cpp $(DEPS)
	g++ -o $(Intermediates)/main.o -c $(SrcDir)/main.cpp $(Options)

$(Intermediates)/math_syntax.o: $(SrcDir)/math_syntax.cpp $(DEPS)
	g++ -o $(Intermediates)/math_syntax.o -c $(SrcDir)/math_syntax.cpp $(Options)

$(Intermediates)/expression_tree.o: $(SrcDir)/expression_tree.cpp $(DEPS)
	g++ -o $(Intermediates)/expression_tree.o -c $(SrcDir)/expression_tree.cpp $(Options)

$(Intermediates)/expression_loader.o: $(SrcDir)/expression_loader.cpp $(DEPS)
	g++ -o $(Intermediates)/expression_loader.o -c $(SrcDir)/expression_loader.cpp $(Options)

$(Intermediates)/expression_simplifier.o: $(SrcDir)/expression_simplifier.cpp $(DEPS)
	g++ -o $(Intermediates)/expression_simplifier.o -c $(SrcDir)/expression_simplifier.cpp $(Options)

$(Intermediates)/differentiation.o: $(SrcDir)/differentiation.cpp $(DEPS)
	g++ -o $(Intermediates)/differentiation.o -c $(SrcDir)/differentiation.cpp $(Options)