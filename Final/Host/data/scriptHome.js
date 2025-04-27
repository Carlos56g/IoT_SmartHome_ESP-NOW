const accsData = {
    key: "",
    mode: "",
    keys: [],
    status: "",
    date: ""
};

const tempData = {
    desiredTemperature: 0,
    actualTemperature: 0,
    actualHumidity: 0,
    mode: "",
    status: "",
    tempDataProg: {
        mode: "",
        onDate: "",
        offDate: "",
        desiredTemperature: 0
    }
};

const modes = {
    AccsNFC: 'D',
    accsOpen: 'E',
    accsClose: 'F',
    off: 'G',
    on: 'H',
    createKey: 'I',
    autoMode: 'O',
    hot: 'P',
    cold: 'Q',
    nothing: 'S',
    air: 'R',
    presence: 'T',
    presenceAndAuto: 'V',
};

const lightData = {
    mode: "",
    onDate: "",
    offDate: "",
    pMode: "",
    desiredBrightness: 0,
    defaultTimeOn: '',
};


function toggleOnOffTemp() {
    const checkbox = document.getElementById("OnOffTemp");
    return checkbox.checked;
}

function toggleOnOffAccs() {
    const checkbox = document.getElementById("OnOffAccs");
    return checkbox.checked;
}

function toggleOnOffLight() {
    const checkbox = document.getElementById("OnOffLight");
    return checkbox.checked;
}


function OnOffAccs() {
    if (toggleOnOffAccs()) {
        accsOn();
        document.getElementById("Accs").className="enabled-style";
    }
    else {
        accsOff();
        document.getElementById("Accs").className="disabled-style";
    }
}

function OnOffTemp() {
    if (toggleOnOffTemp()) {
        tempOn();
        document.getElementById("Temp").className="enabled-style";
    }
    else {
        tempOff();
        document.getElementById("Temp").className="disabled-style";
    }
}

function OnOffLight() {
    if (toggleOnOffLight()) {
        lightOn();
        document.getElementById("Light").className="enabled-style";
    }
    else {
        lightOff();
        document.getElementById("Light").className="disabled-style";
    }
}


function lightOff() {
    // Enviar la petición GET con un header personalizado
    fetch("http://192.168.31.191/api/Light/Mode", {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ on: false })
    })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
}

function lightOn() {
    // Enviar la petición GET con un header personalizado
    fetch("http://192.168.31.191/api/Light/Mode", {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ on: true })
    })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
}


function accsOff() {
    accsData.mode = modes.off;
    // Enviar la petición GET con un header personalizado
    fetch("http://192.168.31.191/api/Accs/Mode", {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(accsData)
    })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
}

function accsOn() {
    accsData.mode = modes.on;
    // Enviar la petición GET con un header personalizado
    fetch("http://192.168.31.191/api/Accs/Mode", {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(accsData)
    })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));

}

function tempOff() {
    tempData.status = modes.off;

    fetch("http://192.168.31.191/api/Temp/Mode", {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(tempData)
    })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
}

function tempOn() {
    tempData.status = modes.on;
    fetch("http://192.168.31.191/api/Temp/Mode", {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(tempData)
    })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
}

function getTempData() {
    fetch('/api/Temp/get')
        .then(response => response.json())
        .then(data => {
            const tbodyTemp = document.getElementById('actualTemp');
            tbodyTemp.innerHTML = "";
            const actualTempH2 = document.createElement('H2');
            actualTempH2.innerHTML = data.actualTemperature;
            tbodyTemp.appendChild(actualTempH2);

            const tbodyHum = document.getElementById('actualHum');
            tbodyHum.innerHTML = "";
            const actualHumH2 = document.createElement('H2');
            actualHumH2.innerHTML = data.actualHumidity;
            tbodyHum.appendChild(actualHumH2);

            const tbodyTempMode = document.getElementById('actualTempMode');
            tbodyTempMode.innerHTML = "";
            const actualTempModeH2 = document.createElement('H2');
            switch (data.mode) {
                case 79: // 'O'
                    actualTempModeH2.innerHTML = "Automático";
                    break;
                case 80: // 'P'
                    actualTempModeH2.innerHTML = "Calentar";
                    break;
                case 81: // 'Q'
                    actualTempModeH2.innerHTML = "Enfriar";
                    break;
                case 82: // 'R'
                    actualTempModeH2.innerHTML = "Solo Aire";
                    break;
            }
            tbodyTempMode.appendChild(actualTempModeH2);
        })
        .catch(error => console.error("Error al obtener la temperatura actual:", error));
}

function getLightsData() {
    updateInput = true;
    fetch('/api/Lights/get')
        .then(response => response.json())
        .then(data => {
            const tbody = document.getElementById('lightsInfoBody');
            tbody.innerHTML = "";
            data.forEach((light, index) => {
                const row = document.createElement('tr');

                var modeDescription;
                switch (light.mode) {
                    case 84: // 'T'
                        modeDescription = "Presencia";
                        break;
                    case 79: // 'O'
                        modeDescription = "Luz Ambiente";
                        break;
                    case 86: // 'V'
                        modeDescription = "Presencia & Luz Ambiente";
                        break;
                    case 72: // 'H'
                        modeDescription = "Encendido";
                        break;
                    case 71: // 'G'
                        modeDescription = "Apagado";
                        break;
                }

                row.innerHTML = `
            <td>${index + 1}</td>
            
            <td> ${modeDescription}</td>
            
            <td>${light.state}</td>`
                    ;
                tbody.appendChild(row);
            });
        })
        .catch(error => console.error("Error al obtener la informacion de las luces:", error));
}

function getAccsData() {
    fetch('api/Accs/get')
        .then(response => response.json())
        .then(data => {
            const tbodyaccsKey = document.getElementById('accsKey');
            tbodyaccsKey.innerHTML = "";
            const actualKey = document.createElement('H2');
            actualKey.innerHTML = data.key;
            tbodyaccsKey.appendChild(actualKey);

            const tbodyaccsStatus = document.getElementById('accsStatus');
            tbodyaccsStatus.innerHTML = "";
            const actualaccsStatus = document.createElement('H2');
            switch (data.status) {
                case 74: // 'J'
                    actualaccsStatus.innerHTML = "Aceptado";
                    break;
                case 75: // 'K'
                    actualaccsStatus.innerHTML = "Denegado";
                    break;
                case 76: // 'L'
                    actualaccsStatus.innerHTML = "Esperando Llave para Grabar";
                    break;
                case 78: // 'N'
                    actualaccsStatus.innerHTML = "Usuario Registrado";
                    break;
                case 71: // 'G'
                    actualaccsStatus.innerHTML = "Apagado";
                    break;
                case 72: // 'H'
                    actualaccsStatus.innerHTML = "Encendido";
                    break;
                case 69: // 'E'
                    actualaccsStatus.innerHTML = "Abierto";
                    break;
                case 70: // 'F'
                    actualaccsStatus.innerHTML = "Cerrado";
                    break;
                case 65: // 'A'
                    actualaccsStatus.innerHTML = "Delete Keys";
                    break;
            }
            tbodyaccsStatus.appendChild(actualaccsStatus);

            const tbodyaccsDate = document.getElementById('accsDate');
            tbodyaccsDate.innerHTML = "";
            const actualAccsDate = document.createElement('H2');
            actualAccsDate.innerHTML = data.date;
            tbodyaccsDate.appendChild(actualAccsDate);

            const tbodyaccsMode = document.getElementById('accsMode');
            tbodyaccsMode.innerHTML = "";
            const actualaccsMode = document.createElement('H2');
            switch (data.mode) {
                case 68: // 'D'
                    actualaccsMode.innerHTML = "Acceso con NFC";
                    break;
                case 71: // 'G'
                    actualaccsMode.innerHTML = "Apagado";
                    break;
                case 72: // 'H'
                    actualaccsMode.innerHTML = "Encendido";
                    break;
                case 69: // 'E'
                    actualaccsMode.innerHTML = "Abierto";
                    break;
                case 70: // 'F'
                    actualaccsMode.innerHTML = "Cerrado";
                    break;
                case 73: // 'I'
                    actualaccsMode.innerHTML = "Crear Llave";
                    break;
            }
            tbodyaccsMode.appendChild(actualaccsMode);
        })
}


//Obtendra la temperatura y el Estado de la Luces cada 2s
setInterval(getTempData, 2000);
setInterval(getLightsData, 2000);
setInterval(triggerAccsAction, 2000);


//Se encargara de revisar si hay cambios en el modulo de acceso, no es viable hacer uso de SetInterval debido a como fue programada
async function triggerAccsAction() {
    try {
      const response = await fetch('/api/Accs/trigger');
      if (response.status === 200) {
        getAccsData();
      }
    } catch (error) {
      console.error('Error en la solicitud:', error);
    }
  }

function reload(){
    const confirmar = confirm('¿Estás seguro que quieres reiniciar los Dispositivos?');
    if (confirmar) {
        fetch('/reload')
    }
}

