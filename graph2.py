import matplotlib.pyplot as plt
import os

# ==============================
# CONFIG
# ==============================

Ns = [1, 10, 100, 1000]
configs = [(1,1), (1,2), (1,4), (1,8), (2,1), (4,1), (8,1)]

output_dir = "graficos_buffer"

# cria pasta se não existir
os.makedirs(output_dir, exist_ok=True)

# ==============================
# FUNÇÃO
# ==============================

def plot_buffer(file_name, titulo, save_path):
    ocupacao = []

    with open(file_name) as f:
        for linha in f:
            ocupacao.append(int(linha.strip()))

    plt.figure()
    plt.plot(ocupacao)
    plt.xlabel("Operações")
    plt.ylabel("Ocupação do buffer")
    plt.title(titulo)
    plt.grid()
    plt.tight_layout()

    plt.savefig(save_path)
    plt.close()  # importante pra não travar com muitos gráficos


# ==============================
# GERAR TODOS
# ==============================

for N in Ns:
    for (Np, Nc) in configs:
        filename = f"dados/ocupacao_N{N}_Np{Np}_Nc{Nc}.txt"

        if not os.path.exists(filename):
            print(f"Arquivo não encontrado: {filename}")
            continue

        titulo = f"N={N}, Np={Np}, Nc={Nc}"
        save_path = os.path.join(
            output_dir,
            f"buffer_N{N}_Np{Np}_Nc{Nc}.png"
        )

        plot_buffer(filename, titulo, save_path)

print(f"\n Gráficos salvos na pasta: {output_dir}")