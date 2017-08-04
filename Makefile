
OBJECT_DIR = obj
SRC_DIR    = srctmp
BIN_DIR    = bin

OBJECTS  = $(OBJECT_DIR)/biclique.o
OBJECTS += $(OBJECT_DIR)/bigraph.o
OBJECTS += $(OBJECT_DIR)/utility.o

EXEC_NAMES = meba

EXECS = $(addprefix $(BIN_DIR)/, $(EXEC_NAMES))

CC = g++
DEFINE = -std=c++11
DEFINE += -g
#DEFINE += -DPERFORMANCE
DEFINE += -O3

VPATH = src

.PHONY : all

all: $(EXECS)

.PHONY : clean

clean: 
	rm -rf $(OBJECTS) $(EXECS) $(OBJECT_DIR)  $(SRC_DIR); mkdir $(OBJECT_DIR)  $(SRC_DIR) ; cp src/*.cpp $(SRC_DIR); cp src/*.h $(SRC_DIR) 

$(BIN_DIR)/meba: main.cpp ${OBJECTS} 
	${CC} ${DEFINE} ${OBJECTS} $(SRC_DIR)/main.cpp  -o $@

$(OBJECT_DIR)/biclique.o: biclique.cpp biclique.h 
	${CC} ${DEFINE} -c $(SRC_DIR)/biclique.cpp -o $@

$(OBJECT_DIR)/bigraph.o: bigraph.cpp bigraph.h utility.h
	${CC} ${DEFINE} -c $(SRC_DIR)/bigraph.cpp -o $@

$(OBJECT_DIR)/utility.o: utility.cpp utility.h 
	${CC} ${DEFINE} -c $(SRC_DIR)/utility.cpp -o $@


