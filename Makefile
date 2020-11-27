Options = -Wall -Wpedantic

#    (sin (      5)) + ( ( x  ) * (10     )   )    

SrcDir = src
BinDir = bin
Intermediates = $(BinDir)/intermediates
LibDir = libs

LIBS = $(LibDir)/file_manager.a $(LibDir)/log_generator.a
DEPS = $(SrcDir)/expression_tree.h $(LibDir)/file_manager.h $(LibDir)/log_generator.h

$(BinDir)/deriv_calc.exe: $(Intermediates)/main.o $(Intermediates)/expression_tree.o $(LIBS) $(DEPS)
	g++ -o $(BinDir)/deriv_calc.exe $(Intermediates)/main.o $(Intermediates)/expression_tree.o $(LIBS)

$(Intermediates)/main.o: $(SrcDir)/main.cpp $(DEPS)
	g++ -o $(Intermediates)/main.o -c $(SrcDir)/main.cpp $(Options)

$(Intermediates)/expression_tree.o: $(SrcDir)/expression_tree.cpp $(DEPS)
	g++ -o $(Intermediates)/expression_tree.o -c $(SrcDir)/expression_tree.cpp $(Options)