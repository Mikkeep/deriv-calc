Options = -Wall -Wpedantic

SrcDir = src
BinDir = bin
Intermediates = $(BinDir)/intermediates
LibDir = libs

LIBS = $(LibDir)/file_manager.a $(LibDir)/log_generator.a
DEPS = $(SrcDir)/binary_tree.h $(LibDir)/file_manager.h $(LibDir)/log_generator.h

$(BinDir)/deriv_calc.exe: $(Intermediates)/main.o $(Intermediates)/binary_tree.o $(LIBS) $(DEPS)
	g++ -o $(BinDir)/deriv_calc.exe $(Intermediates)/main.o $(Intermediates)/binary_tree.o $(LIBS)

$(Intermediates)/main.o: $(SrcDir)/main.cpp $(DEPS)
	g++ -o $(Intermediates)/main.o -c $(SrcDir)/main.cpp $(Options)

$(Intermediates)/binary_tree.o: $(SrcDir)/binary_tree.cpp $(DEPS)
	g++ -o $(Intermediates)/binary_tree.o -c $(SrcDir)/binary_tree.cpp $(Options)