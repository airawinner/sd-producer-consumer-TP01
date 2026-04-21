# ==============================
# CONFIG
# ==============================

CXX = g++
CXXFLAGS = -std=c++20 -O2 -pthread

TARGET = programa
SRC = pro_con_sem.cpp

PYTHON = python3
SCRIPT1 = txt_graph1.py
SCRIPT2 = graph2.py

# ==============================
# BUILD
# ==============================

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# ==============================
# RUN (executa tudo)
# ==============================

run: all
	@echo " Rodando experimentos..."
	$(PYTHON) $(SCRIPT1)
	@echo " Gerando gráficos..."
	$(PYTHON) $(SCRIPT2)

# ==============================
# CLEAN
# ==============================

clean:
	rm -f $(TARGET)
	rm -f *.csv
	rm -f *.png
	rm -f ocupacao_*.txt
	rm -rf graficos_buffer