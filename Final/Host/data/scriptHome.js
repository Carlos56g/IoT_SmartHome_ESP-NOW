var tempMode;
var accsMode;
var lightsMode;

var alertTemp=false;
var alertAccs=false;
var alertLights=false;

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


function desAccs() {
    div = document.getElementById("Accs");
    div.style.pointerEvents = 'none';
    document.getElementById("Accs").className = "disabled-style";
}

function desTemp() {
    div = document.getElementById("Temp");
    div.style.pointerEvents = 'none';
    document.getElementById("Temp").className = "disabled-style";
}

function desLights() {
    div = document.getElementById("Light");
    div.style.pointerEvents = 'none';
    document.getElementById("Light").className = "disabled-style";
}

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
    }
    else {
        accsOff();
    }
}

function OnOffTemp() {
    if (toggleOnOffTemp()) {
        tempOn();
    }
    else {
        tempOff();
    }
}

function OnOffLight() {
    if (toggleOnOffLight()) {
        lightOn();
    }
    else {
        lightOff();
    }
}


function lightOff() {
    const checkbox = document.getElementById("OnOffLight");
    checkbox.checked=false;
    // Enviar la petición GET con un header personalizado
    fetch("/api/Light/Mode", {
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
    fetch("/api/Light/Mode", {
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
    const checkbox = document.getElementById("OnOffAccs");
    checkbox.checked=false;
    accsData.mode = modes.off;
    // Enviar la petición GET con un header personalizado
    fetch("/api/Accs/Mode", {
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
    fetch("/api/Accs/Mode", {
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
    const checkbox = document.getElementById("OnOffTemp");
    checkbox.checked=false;
    tempData.status = modes.off;
    fetch("/api/Temp/Mode", {
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
    fetch("/api/Temp/Mode", {
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
        .then(response => {
            if (!response.ok) {
                throw new Error("Error al obtener datos del módulo de Temperatura");
            }
            return response.json();
        })
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
                case 79:
                    actualTempModeH2.innerHTML = "Automático";
                    break;
                case 80:
                    actualTempModeH2.innerHTML = "Calentar";
                    break;
                case 81:
                    actualTempModeH2.innerHTML = "Enfriar";
                    break;
                case 82:
                    actualTempModeH2.innerHTML = "Solo Aire";
                    break;
            }
            tbodyTempMode.appendChild(actualTempModeH2);
            if(data.status==71){
                tempOff();
            }
        })
        .catch(error => {
            if(!alertTemp){
                const checkboxTemp = document.getElementById("OnOffTemp");
                checkboxTemp.disabled = false;
                desTemp();
                alert(error.message || "Error inesperado al obtener datos de temperatura");
                alertTemp=true;
            }
        });
}



function getLightsData() {
    fetch('/api/Lights/get')
        .then(response => {
            if (!response.ok) {
                throw new Error("Error al obtener datos del módulo de Luces");
            }
            return response.json();
        })
        .then(data => {
            const tbody = document.getElementById('lightsInfoBody');
            tbody.innerHTML = "";
            data.lightDevices.forEach((light, index) => {
                const row = document.createElement('tr');

                let modeDescription;
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
                    default:
                        modeDescription = "Desconocido";
                }

                row.innerHTML = `
                    <td>${index + 1}</td>
                    <td>${modeDescription}</td>
                    <td>${light.state}</td>
                `;
                tbody.appendChild(row);
            });
            if(data.on==false){
                lightOff();
            }
        })
        .catch(error => {
            if (!alertLights) {
                const checkboxLight = document.getElementById("OnOffLight");
                checkboxLight.disabled = false;
                desLights();
                alert(error.message || "Error inesperado al obtener datos de luces");
                alertLights = true;
            }
        });
}


function getAccsData() {
    fetch('/api/Accs/get')
        .then(response => {
            if (!response.ok) {
                throw new Error("Error al obtener datos del módulo de Acceso");
            }
            return response.json();
        })
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
                case 74: actualaccsStatus.innerHTML = "Aceptado"; break;
                case 75: actualaccsStatus.innerHTML = "Denegado"; break;
                case 76: actualaccsStatus.innerHTML = "Esperando Llave para Grabar"; break;
                case 78: actualaccsStatus.innerHTML = "Usuario Registrado"; break;
                case 71: actualaccsStatus.innerHTML = "Apagado"; break;
                case 72: actualaccsStatus.innerHTML = "Encendido"; break;
                case 69: actualaccsStatus.innerHTML = "Abierto"; break;
                case 70: actualaccsStatus.innerHTML = "Cerrado"; break;
                case 65: actualaccsStatus.innerHTML = "Delete Keys"; break;
                default: actualaccsStatus.innerHTML = "Desconocido";
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
                case 68: actualaccsMode.innerHTML = "Acceso con NFC"; break;
                case 71: actualaccsMode.innerHTML = "Apagado"; break;
                case 72: actualaccsMode.innerHTML = "Encendido"; break;
                case 69: actualaccsMode.innerHTML = "Abierto"; break;
                case 70: actualaccsMode.innerHTML = "Cerrado"; break;
                case 73: actualaccsMode.innerHTML = "Crear Llave"; break;
                default: actualaccsMode.innerHTML = "Desconocido";
            }
            tbodyaccsMode.appendChild(actualaccsMode);
            if(data.mode==71){
                accsOff();
            }
        })
        .catch(error => {
            if (!alertAccs) {
                const checkboxAccs = document.getElementById("OnOffAccs");
                checkboxAccs.disabled = false;
                desAccs();
                alert(error.message || "Error inesperado al obtener datos de acceso");
                alertAccs = true;
            }
        });
}

setInterval(getTempData, 2000);

setInterval(getLightsData, 2000);

setInterval(triggerAccsAction, 2000);



//Se encargara de revisar si hay cambios en el modulo de acceso, no es viable hacer uso de SetInterval debido a como fue programada
async function triggerAccsAction() {
    fetch('/api/Accs/trigger')
        .then(response => {
            if (!response.ok) {
                throw new Error("Error al obtener datos del módulo de Acceso");
            }
            if (response.ok) {
                getAccsData();
            }
        })
        .catch(error => {
            if (!alertAccs) {
                const checkboxAccs = document.getElementById("OnOffAccs");
                checkboxAccs.disabled = false;
                desAccs();
                alert(error.message || "Error inesperado al obtener datos de acceso");
                alertAccs = true;
            }
        });
}

function reload() {
    const confirmar = confirm('¿Estás seguro que quieres reiniciar los Dispositivos?');
    if (confirmar) {
        fetch('/reload')
    }
}

