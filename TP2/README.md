```
 ______   ______     ______     ______     ______     __         __  __     ______       
/\__  _\ /\  == \   /\  __ \   /\  == \   /\  __ \   /\ \       /\ \_\ \   /\  __ \      
\/_/\ \/ \ \  __<   \ \  __ \  \ \  __<   \ \  __ \  \ \ \____  \ \  __ \  \ \ \/\ \     
   \ \_\  \ \_\ \_\  \ \_\ \_\  \ \_____\  \ \_\ \_\  \ \_____\  \ \_\ \_\  \ \_____\    
    \/_/   \/_/ /_/   \/_/\/_/   \/_____/   \/_/\/_/   \/_____/   \/_/\/_/   \/_____/    
                                                                                         
 ______   ______     ______     ______   __     ______     ______        __     __       
/\  == \ /\  == \   /\  __ \   /\__  _\ /\ \   /\  ___\   /\  __ \      /\ \   /\ \      
\ \  _-/ \ \  __<   \ \  __ \  \/_/\ \/ \ \ \  \ \ \____  \ \ \/\ \     \ \ \  \ \ \     
 \ \_\    \ \_\ \_\  \ \_\ \_\    \ \_\  \ \_\  \ \_____\  \ \_____\     \ \_\  \ \_\    
  \/_/     \/_/ /_/   \/_/\/_/     \/_/   \/_/   \/_____/   \/_____/      \/_/   \/_/    
                                                                                         
```

# Trabalho Prático 2 — Visualizador e Editor 3D

<details>
<summary><strong>Descrição e Requisitos</strong></summary>

O formato de arquivo OBJ é um dos formatos de arquivos comumente utilizados para definir a geometria de um objeto 3D. Inicialmente desenvolvido pela Wavefront Technologies para seu pacote de visualização, o formato é aberto e suportado por muitos softwares de modelagem e visualização 3D, incluindo o Blender.

Construa uma aplicação gráfica que permita ao usuário visualizar e editar uma cena 3D formada a partir das geometrias Box, Cylinder, Sphere, GeoSphere, Grid, Quad e também geometrias carregadas a partir de arquivos OBJ.

### Tabela 1 — Teclas para criar figuras:
| Tecla | Figura     |
|-------|------------|
| B     | Box        |
| C     | Cylinder   |
| S     | Sphere     |
| G     | GeoSphere  |
| P     | Plane(Grid)|
| Q     | Quad       |

### Tabela 2 — Teclas para ações:
| Tecla   | Ação                        |
|---------|-----------------------------|
| 1 a 5   | Carregar modelos de teste   |
| TAB     | Seleciona objeto            |
| DEL     | Remove objeto               |
| V       | Alterna modo de visualização|

- Use teclado e/ou mouse para translação, escala e rotação da figura selecionada.
- O objeto selecionado deve usar uma cor distinta dos demais.
- A tecla V alterna para o modo de 4 vistas: **Front, Top, Right, Perspective**.
- Exceto a perspectiva, as outras vistas usam projeção ortográfica.
- Apenas a câmera da perspectiva é interativa (gira/zoom com mouse).
- Teclas 1 a 5 carregam modelos de teste: Ball, Capsule, House, Monkey, Thorus.
- O grid da cena é sempre exibido, não pode ser selecionado ou removido.

#### Critérios de avaliação:
- Modo de visualização em 4 vistas
- Linhas divisórias no modo de 4 vistas
- Carregamento de modelos 3D de arquivos OBJ
- Adição, exclusão e seleção de objetos na cena
- Translação, escala e rotação de objetos

</details>

---

<details>
<summary><strong>Checklist de Implementação</strong></summary>

### Tarefas

- [ ] Implementar modo de visualização em 4 vistas (Front, Top, Right, Perspective)
    - [ ] Calcular largura e altura de cada quadrante com base no tamanho da janela
    - [ ] Definir quatro viewports (D3D12_VIEWPORT), um para cada quadrante
    - [ ] Definir quatro scissor rects (D3D12_RECT), um para cada quadrante
    - [ ] Refatorar o loop de desenho para iterar sobre os quatro viewports
    - [ ] Em cada iteração:
        - [ ] Setar o viewport correspondente
        - [ ] Setar o scissor rect correspondente
        - [ ] Definir a câmera correta para a vista:
            - [ ] Perspective (câmera orbital, topo esquerdo)
            - [ ] Top (visão de cima, topo direito)
            - [ ] Front (visão frontal, baixo esquerdo)
            - [ ] Right (visão lateral direita, baixo direito)
        - [ ] Atualizar a matriz de visualização/projeção para cada vista
        - [ ] Atualizar o constant buffer de cada objeto para cada viewport
        - [ ] Renderizar todos os objetos da cena
    - [ ] Permitir alternar entre modo de uma vista e modo de quatro vistas
    - [ ] Testar se cada quadrante está desenhando corretamente e sem vazamento

- [ ] Adicionar linhas divisórias entre as vistas para melhor separação visual
    - [ ] Calcular as posições das linhas divisórias com base no tamanho da janela
    - [ ] Desenhar linhas verticais e horizontais entre os quadrantes após renderizar as cenas
    - [ ] Garantir que as linhas fiquem sempre visíveis, independente do conteúdo das vistas

- [ ] Carregar modelos 3D de arquivos OBJ
    - [ ] Implementar parser simples para arquivos OBJ
    - [ ] Adicionar carregamento de modelo OBJ via atalho
    - [ ] Adicionar modelo carregado à cena como novo objeto
    - [ ] Testar carregamento com diferentes arquivos OBJ

- [x] Permitir adição, exclusão e seleção de objetos na cena (teclado)
    - [x] Implementar lógica para adicionar objetos (primitivas)
    - [x] Implementar lógica para remover objetos selecionados
    - [x] Implementar seleção de objetos via TAB
    - [x] Destacar visualmente o objeto selecionado

- [x] Implementar translação, escala e rotação de objetos selecionados
    - [x] Implementar manipulação de translação via teclado
    - [x] Implementar manipulação de escala via teclado
    - [x] Implementar manipulação de rotação via teclado
    - [x] Atualizar matriz de transformação do objeto selecionado em tempo real
    - [x] Garantir que apenas o objeto selecionado seja transformado

### Opcional

- [x] (Opcional) Saída de debug no console (eventos de seleção, inserção, remoção)
- [ ] (Opcional) Implementar seleção de objetos via clique do mouse (picking)
- [ ] (Opcional) Usar scissor rects para garantir que cada viewport desenhe apenas dentro do seu quadrante, evitando vazamento de desenho
- [ ] (Opcional) Permitir redimensionamento dinâmico das viewports conforme o tamanho da janela

</details>

---

<details>
<summary><strong>Ideias de Melhoria</strong></summary>



</details>

---

## Observações sobre Debug Console

- O programa pode exibir mensagens de debug no console (opcional), mostrando eventos como seleção, inserção e remoção de objetos.
- Para ativar/desativar, ajuste o código em WinMain e Update conforme desejado.
- O grid da cena é sempre exibido, não pode ser selecionado ou removido.

---

## Como implementar e por quê

- **Viewports e scissor rects:**  
  Cada viewport define a área de projeção da cena em um quadrante da janela. O scissor rect atua como uma “máscara” de recorte, garantindo que nada seja desenhado fora do quadrante correspondente, mesmo que a geometria ultrapasse os limites.
- **Loop de desenho:**  
  Iterar sobre os quatro viewports, ajustando viewport, scissor e câmera antes de desenhar a cena. Isso garante que cada vista seja renderizada corretamente e de forma independente.
- **Linhas divisórias:**  
  Melhoram a separação visual entre as vistas, facilitando a identificação de cada quadrante.
- **Sem bibliotecas externas:**  
  Todas as implementações devem usar apenas Direct3D 12, DirectXMath e o código já existente no projeto.

---

## Licença

Este projeto é acadêmico e segue as diretrizes da disciplina de Computação Gráfica.

---
