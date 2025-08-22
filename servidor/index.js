// Importar la clase SerialPort y DelimiterParser del módulo 'serialport'
const express = require('express');
//const { createServer } = require('node:http');
const http = require('http');  //crea servidor http
const socketIo = require('socket.io');
const SerialPort = require('serialport').SerialPort;
 // Importar la clase DelimiterParser del módulo 'serialport'
const { DelimiterParser } = require('@serialport/parser-delimiter');

// Crear una aplicación Express y un servidor HTTP
const app = express();
const server = http.createServer(app);
const io = socketIo(server);

// Configurar el servidor Express para servir archivos estáticos desde el directorio 'public'
app.use(express.static(__dirname + '/public'));

// Configurar el objeto de conexión serial con el puerto COM9 y una velocidad de baudios de 9600
const puerto = new SerialPort({
    path: 'COM7',
    baudRate: 9600
});

// Configurar el parser para dividir los datos en líneas ('\n')
const parser = puerto.pipe(new DelimiterParser({ delimiter: '\n' }));

// Manejar el evento 'open' cuando se abre la conexión
parser.on('open', function () {
    console.log('Conexión abierta');
});

//io.on('connection', (socket) => {
  //  console.log('Cliente conectado');
    // Manejar el evento 'data' cuando se reciben datos desde el puerto serial
    parser.on('data', (data) => {
      const enc = new TextDecoder(); // Crear un decodificador de texto
      const arr = new Uint8Array(data); // Convertir los datos a un array de enteros sin signo de 8 bits
      const ready = enc.decode(arr); // Decodificar los datos a texto
      
      // Enviar los datos al cliente a través de Socket.io

         // Mostrar los datos recibidos en la consola
      console.log(ready);
      //socket.emit('data', ready);
      // Emitir los datos a todos los clientes conectados a través de Socket.IO con el evento 'ready'
      io.emit('ready', data.toString());
    });
//  });


  server.listen(3000, () => {
    console.log('server running at http://localhost:3000');
  });

