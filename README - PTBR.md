# ArduinoHourglassLCD
 Ampulheta Digital com Arduino - Usa um Display LCD para a configuração do tempo.
## Componentes usados:
 - Arduino Uno
 - 2 Matrizes LED 8x8 com controlador MAX7219;
 - Display LCD 20x4;
 - 2 pushbuttons (Resistor Pull-Up é interno);
 - Acelerômetro MPU6050;
 - Buzzer Passivo (não é necessário para as funções básicas).
## Como usar:
### Bibliotecas Arduino Necessárias:
Para poder compilar o código, você terá que instalar as bibliotecas "LiquidCrystal", "Wire" e "MPU6050_tockn". Os arquivos LedControl.h e Delay.h estão incluídos na pasta, eu recomendo que você baixe [a pasta completa](/Ampulheta-V3-LCD/) e compile a partir disso.
### Conexões do Circuito:
Aqui temos um diagrama simplificado mostrando todas as conexões necessárias para fazer funcionar. Note que o buzzer é opcional e pode ser removido.
![](/Diagrama-PTBR.jpg)
## Como funciona:
Ao ligar, uma mensagem no Display LCD aparece, conforme o acelerômetro é calibrado. Ao fim da calibração, a sequência começa. Se a ampulheta estiver "de pé", ela começará a contar. Ela possui três modos, os quais podem ser 'selecionados' ao apertar o botão colocado no pino 3:
- Config. Minutos
- Config. Segundos
- Ampulheta
### Config. Minutos:
Este modo torna possível aumentar ou diminuir o valor armazenado nos Minutos. Para aumentar, aperte o botão colocado no pino 2. Para diminuir, dê uma apertada "curta" no botão colocado no pino 3. Note que a duração de um aperto "longo" é determinado pela variável "DEBOUNCE_THRESHOLD" no código principal. Os valores do tempo serão mostrados no display LCD. 
### Config. Segundos:
Possui o mesmo comportamento que Config. Minutos, mas mexe com o valor armazenado nos Segundos. O tempo padrão é de 20 segundos, o qual pode ser substituído por 'qualquer valor'. Eu escolhi um valor 'pequeno' para que pudesse ser fácil de inspecionar funções básicas do código durante o desenvolvimento. 
### Ampulheta:
O modo padrão e o principal. Aqui é onde tudo acontece - "partículas" caem da matriz de cima para a matriz de baixo, as quais são determinadas pela leitura do acelerômetro. Um conjunto de funções é usado para dizer se as "partículas" podem se mover ou não, permitindo também que ao "deitar" a ampulheta de lado, nenhum movimento seja observado.

## "Inspiração" Original:
O código original vem [deste projeto](https://www.instructables.com/Arduino-Hourglass/). A principal diferença entre aquela versão e esta é que algumas funções foram renomeadas, o MPU6050 foi adicionado, além da presença do duisplay LCD. 

## To-Do:
- [X] Translate this README to Brazilian Portuguese
- [ ] Translate the text shown in the LCD display and Serial prints to English
- [ ] Check all three boxes when everything is done :D