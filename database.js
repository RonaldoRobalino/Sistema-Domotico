var database = firebase.database();
var ref = database.ref('domotica-pro');
ref.on('value', function(ss) {

    var LED = ss.val()
    console.log(LED);
    document.getElementById('sensor1').innerHTML.LED.Distancia;
    document.getElementById('sensor2').innerHTML.LED.Humedad;
    document.getElementById('sensor3').innerHTML.LED.Temperatura;
    document.getElementById('sensor4').innerHTML.LED.PIR;

});