import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import glob
import shutil

# ==============================
# CONFIG
# ==============================
EXECUTABLE = "./programa"

Ns = [1, 10, 100, 1000]
configs = [(1,1), (1,2), (1,4), (1,8), (2,1), (4,1), (8,1)]
labels = list("abcdefg")

os.makedirs("graficos", exist_ok=True)
os.makedirs("dados", exist_ok=True)

results = []

print(" Iniciando experimentos (10 execuções cada)...\n")

# ==============================
# EXECUÇÕES (10x por config)
# ==============================
for N in Ns:
    for i, (Np, Nc) in enumerate(configs):

        print("====================================")
        print(f"Configuração: N={N}, Np={Np}, Nc={Nc}")
        print("------------------------------------")

        tempos = []

        for run in range(10):
            process = subprocess.run(
                [EXECUTABLE, str(N), str(Np), str(Nc)],
                capture_output=True,
                text=True
            )

            try:
                tempo = float(process.stdout.strip())
                tempos.append(tempo)
                print(f"Execução {run+1}: {tempo:.6f} s")
            except:
                print(f"Execução {run+1}: ERRO")

        # salva os 10 tempos
        filename = f"dados/tempos_N{N}_Np{Np}_Nc{Nc}.txt"
        with open(filename, "w") as f:
            for t in tempos:
                f.write(f"{t}\n")

        # média
        if tempos:
            media = np.mean(tempos)
            print(f"\n➡ Média (10 execuções): {media:.6f} s")
        else:
            media = 0
            print("\n➡ Falha ao calcular média")

        print("\n")

        results.append({
            "N": N,
            "config": labels[i],
            "tempo_medio": media
        })

# ==============================
# MOVER TODOS OS .TXT GERADOS PELO PROGRAMA
# ==============================
txt_files = glob.glob("*.txt")

for file in txt_files:
    destino = os.path.join("dados", file)

    if not os.path.exists(destino):
        shutil.move(file, destino)
    else:
        base, ext = os.path.splitext(file)
        i = 1
        while os.path.exists(os.path.join("dados", f"{base}_{i}{ext}")):
            i += 1
        shutil.move(file, os.path.join("dados", f"{base}_{i}{ext}"))

# ==============================
# DATAFRAME FINAL
# ==============================
df = pd.DataFrame(results)
df.to_csv("dados/medias.csv", index=False)

# ==============================
# GRÁFICO FINAL (ÚNICO)
# ==============================
plt.figure(figsize=(10,6))

for N in Ns:
    subset = df[df["N"] == N]

    x = subset["config"]
    y = subset["tempo_medio"]

    plt.plot(x, y, marker='o', label=f"N={N}")

plt.xlabel("Configurações (a=1P/1C, b=1P/2C, ...)")
plt.ylabel("Tempo médio (s)")
plt.title("Tempo médio vs Configurações (10 execuções)")
plt.legend()
plt.grid()

plt.savefig("graficos/tempo_execucao.png")
plt.show()

print("✔ Tudo pronto: execuções impressas, .txt em /dados e gráfico gerado!")