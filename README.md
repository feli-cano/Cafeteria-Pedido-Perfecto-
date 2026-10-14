# Cafeteria "Pedido Perfecto" - Integracion de Patrones de Diseno

# Nombres de los integrantes: Felipe Muñoz Cano, Emmanuel Bonilla, Laura Posada
## 1. Nombre de la Aplicacion y Problema que Resuelve
* **Nombre:** Cafeteria "Pedido Perfecto"
* **Problema que Resuelve:** En un sistema de pedidos de cafe, la personalizacion de bebidas con multiples extras (leches vegetales, shots adicionales, crema) y la notificacion en tiempo real a diferentes departamentos (barra, caja, pantallas) suelen generar una explosion de subclases o codigo plagado de condicionales `if-else` complejos y dificiles de mantener. La aplicacion resuelve esto mediante una construccion fluida paso a paso, una adicion modular de componentes y un sistema de avisos completamente desacoplado.

## 2. Los Tres Patrones Elegidos (Grupo, Patron y Rol en la Aplicacion)
* **Grupo Creacional - Patron Builder (`PedidoBuilder`):** 
  * *Rol:* Construye el objeto `Pedido` y su bebida paso a paso de manera legible, separando la complejidad de ensamblaje de la representacion final.
* **Grupo Estructural - Patron Decorator (`Extra`, `LecheAvena`, `ShotExtra`, `Crema`):** 
  * *Rol:* Envuelve dinamicamente el componente base (`Cafe`) para anadir responsabilidades (costos y descripciones de extras) en tiempo de ejecucion sin alterar las clases originales ni requerir bloques `if-else`.
* **Grupo de Comportamiento - Patron Observer (`Barra`, `Observador`, `Barista`, `Pantalla`, `Caja`):** 
  * *Rol:* Permite que la barra notifique automaticamente los eventos del pedido ("NUEVO", "LISTO") a los suscriptores independientes sin que los objetos dependan directamente unos de otros.

## 3. Comprobacion de la Integracion de los Patrones (Condiciones del Taller)
La solucion cumple de forma rigurosa con los tres criterios de integracion exigidos:
1. **Comparten una abstraccion y se conectan:** Tanto el *Builder* como el *Decorator* comparten y dependen de la misma interfaz base (`Bebida`). Ademas, el objeto producido por el proceso (el `Pedido` con su bebida ya decorada) es utilizado directamente como argumento por el patron *Observer* en el metodo de notificacion.
2. **Participan en el mismo flujo principal:** En cada ciclo del sistema, el *Builder* ensambla la bebida utilizando el *Decorator*, entrega el pedido terminado a la `Barra`, y esta ultima dispara de inmediato el mecanismo del *Observer* hacia la caja, la pantalla y el barista.
3. **Dependencia critica:** Si se elimina el *Decorator*, se pierde la personalizacion dinamica y habria que volver a escribir codigo lleno de `if-else` anidados; si se elimina el *Builder*, la construccion se vuelve rigida y propensa a errores; si se elimina el *Observer*, la barra tendria que acoplarse rigidamente llamando de forma directa a cada modulo del negocio.

## 4. Diagrama de Relaciones entre los Patrones
```text
 [PedidoBuilder] ---> crea ---> [Pedido] contiene ---> [Bebida (Interfaz)]
                                                         ^
                                                         | (hereda / envuelve)
                                                 [Cafe] / [Extra (Decorador)]
                                                                  ^
                                                 +----------------+----------------+
                                                 | (LecheAvena, ShotExtra, Crema)  |
                                                                                   |
 [Barra (Sujeto)] ---- notifica eventos a ----> [Observador (Interfaz)] <---+---+---+
                                                         ^                 |   |   |
                                             +-----------+-----------+     |   |   |
                                             |           |           |     |   |   |
                                         [Barista]   [Pantalla]    [Caja] --+---+---+