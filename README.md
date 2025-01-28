# Monitoramento Através de Drones Terrestres da Saúde de Transformadores e Sobrecarga da Rede Distribuição de Energia Elétrica por Ruídos e Magnetostricção

## Introdução
A magnetostricção é um fenômeno ferromagnético que ocorre em materiais submetidos a campos magnéticos variáveis, resultando na geração de ruídos característicos. Esse fenômeno tem sido amplamente estudado em aplicações de engenharia elétrica, onde se evidencia a relação entre a magnetostricção e a degradação de transformadores elétricos. Esse efeito se torna especialmente relevante em transformadores elétricos, onde a interação entre o núcleo magnético e a corrente alternada pode produzir vibrações mecânicas detectáveis.

Nas últimas décadas, o estudo dos ruídos emitidos por transformadores tem despertado crescente interesse, tanto por razões ambientais, como a poluição sonora em áreas urbanas, quanto por sua aplicação no diagnóstico da integridade estrutural desses equipamentos. Embora a maior parte dos estudos esteja voltada para o impacto sonoro das estações de distribuição próximas a centros residenciais, há uma demanda crescente por métodos eficazes de diagnóstico da saúde dos transformadores em operação.

Diante desse cenário, este projeto propõe um sistema inovador de monitoramento baseado em um drone terrestre, equipado com um microcontrolador RP2040 e um acoplador acústico direcional. O RP2040 foi escolhido devido ao seu baixo consumo de energia e suporte para múltiplas interfaces de comunicação, facilitando a integração com sensores de áudio, GPS e comunicação sem fio. Esse veículo autônomo, teleguiado por um computador de borda, é capaz de mapear a planta das centrais de distribuição de energia elétrica, realizar a análise acústica dos transformadores e processar os dados em uma base em nuvem. O sistema possibilita um diagnóstico remoto e contínuo, permitindo a avaliação de grandes áreas, como metrópoles e estados inteiros, sem necessidade de intervenção humana direta. 

O drone terrestre recebe comandos do computador de borda, que mantém um mapa atualizado da estação de distribuição e integra dados de um sistema de GPS. Dessa forma, o monitoramento se torna automatizado e preciso, reduzindo riscos operacionais e aumentando a eficiência na detecção de falhas incipientes nos transformadores.

[Aprsentação do Projeto em Vídeo no Youtube](https://youtu.be/AHvlCWeeA88?si=6y2-KHquPNgekdsO)

# Analise de Saude de Transformadores usando Magnectostricção

* https://medium.com/@subirmaity/tinyml-implementation-using-raspberry-pi-pico-geometry-gesture-detection-part-ii-d3d83a5d8cdb

