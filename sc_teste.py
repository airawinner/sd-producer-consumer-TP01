import matplotlib.pyplot as plt

# ==============================
# DADOS (SUAS MÉDIAS)
# ==============================

data = {
    1: {
        "(1,1)": 0.056021,
        "(1,2)": 0.069184,
        "(1,4)": 0.418626,
        "(1,8)": 1.554513,
        "(2,1)": 0.147862,
        "(4,1)": 1.046829,
        "(8,1)": 1.515283,
    },
    10: {
        "(1,1)": 0.155589,
        "(1,2)": 0.101752,
        "(1,4)": 0.176558,
        "(1,8)": 0.335481,
        "(2,1)": 0.230749,
        "(4,1)": 0.260263,
        "(8,1)": 0.441294,
    },
    100: {
        "(1,1)": 0.164182,
        "(1,2)": 0.099070,
        "(1,4)": 0.153115,
        "(1,8)": 0.283124,
        "(2,1)": 0.232537,
        "(4,1)": 0.256340,
        "(8,1)": 0.401367,
    },
    1000: {
        "(1,1)": 0.152194,
        "(1,2)": 0.118128,
        "(1,4)": 0.154714,
        "(1,8)": 0.266686,
        "(2,1)": 0.225987,
        "(4,1)": 0.255458,
        "(8,1)": 0.412311,
    }
}

# Ordem fixa das configurações
configs = ["(1,1)", "(1,2)", "(1,4)", "(1,8)", "(2,1)", "(4,1)", "(8,1)"]

# ==============================
# PLOT
# ==============================

plt.figure()

for N, values in data.items():
    tempos = [values[c] for c in configs]
    plt.plot(configs, tempos, marker='o', label=f"N={N}")

plt.xlabel("Configuração (Np, Nc)")
plt.ylabel("Tempo médio (s)")
plt.title("Tempo médio vs Threads Produtor/Consumidor")
plt.legend()
plt.grid()

plt.xticks(rotation=45)
plt.tight_layout()
plt.savefig("grafico_desempenho.png")
plt.show()