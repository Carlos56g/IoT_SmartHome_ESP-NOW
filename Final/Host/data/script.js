

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
    tempDataProg:{
        mode: "",
        onDate: "",
        offDate: "",
        desiredTemperature: 0
    }
};

const modes = {
    AccsNFC: 'D',
    accsOpen:'E',
    accsClose: 'F',
    off:'G',
    on:'H',
    createKey:'I',
    autoMode: 'O',
    hot: 'P',
    cold: 'Q',
    nothing: 'S',
    air: 'R',
};


function toggleOnOffTemp() {
    const checkbox = document.getElementById("toggleStateTemp");
    const div = document.getElementById("Temp");
    const excludeIds = ["toggleStateTemp"];
    const elements = div.querySelectorAll("input, button","select");

    elements.forEach(el => {
        if(!excludeIds.includes(el.id))
            el.disabled = !checkbox.checked;
    })
    return checkbox.checked;
}



function toggleCreateKeyAccs() {
    const checkbox = document.getElementById("toggleCreateKey");
    const accsKeyInput = document.getElementById("accsKeyInput");
    const createKetButton = document.getElementById("accsKeyButton");
    accsKeyInput.disabled = !checkbox.checked;
    createKetButton.disabled = !checkbox.checked;
}

function toggleOnOffAccs() {
    const checkbox = document.getElementById("toggleSatateAccs");
    const div = document.getElementById("Accs");
    const excludeIds = ["goToHistoryBtn","goToKeysBtn","toggleSatateAccs"];
    const elements = div.querySelectorAll("input, button");

    elements.forEach(el => {
        if(!excludeIds.includes(el.id))
            el.disabled = !checkbox.checked;
    })
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

function accsCreateKey() {
    accsData.mode = modes.createKey;
    accsData.key = document.getElementById("accsKeyInput").value;
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

function accsOpen() {
    accsData.mode = modes.accsOpen;
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
    console.log("Abriendo Acceso");
}

function accsClose() {
    accsData.mode = modes.accsClose;
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
    console.log("Cerrando Acceso");
}


function getAccsHistory() {
    fetch('/api/Accs/History/get')
        .then(response => response.json())
        .then(data => {
            const tbody = document.getElementById('historyBody');
            data.forEach(item => {
                const row = document.createElement('tr');
                row.innerHTML = `<td>${item.key}</td><td>${item.date}</td><td>${item.status}</td>`;
                tbody.appendChild(row);
            });
        })
        .catch(err => console.error("Error al cargar historial:", err));

}

function getAccsKeys() {
    fetch('/api/Accs/Keys/get')
        .then(response => response.json())
        .then(data => {
            const tbody = document.getElementById('keysBody');
            tbody.innerHTML = "";
            data.forEach((key, index) => {
                const row = document.createElement('tr');
                row.innerHTML = `<td>${index + 1}</td><td>${key.key}</td>`;
                tbody.appendChild(row);
            });
        })
        .catch(error => console.error("Error al obtener las llaves:", error));
}

function deleteAccsHistory() {
    fetch('/api/Accs/History/delete', {
      method: 'GET',  // Usamos el método GET para llamar al servidor
    })
    .then(response => response.text())  // Respuesta en formato de texto
    .then(data => {
      console.log("Respuesta del servidor:", data);
      alert(data);  // Muestra la respuesta
    })
    .catch(error => {
      console.error("Error al eliminar el historial:", error);
      alert("Hubo un problema al eliminar el historial.");
    });
  }

function deleteAccsKeys() {
    fetch('/api/Accs/Keys/delete', {
      method: 'GET',  // Usamos el método GET para llamar al servidor
    })
    .then(response => response.text())  // Respuesta en formato de texto
    .then(data => {
      console.log("Respuesta del servidor:", data);
      alert(data);  // Muestra la respuesta
    })
    .catch(error => {
      console.error("Error al eliminar las llaves:", error);
      alert("Hubo un problema al eliminar las llaves.");
    });
  }


function getTempData(){
    updateInput=true;
    fetch('/api/Temp/get')
        .then(response => response.json())
        .then(data => {

            if(window.location.pathname === "/Temp/Prog"){
                const desiredProgTemperatureInput=document.getElementById('desiredProgTemperatureInput');
                desiredProgTemperatureInput.value=data.tempDataProg.desiredTemperature;
                const tempProgModeSelect=document.getElementById('tempProgModeSelect');
                tempProgModeSelect.value=String.fromCharCode(data.tempDataProg.mode);
                const onTempDate=document.getElementById('onTempDate');
                onTempDate.value=data.tempDataProg.onDate;
                const offTempDate=document.getElementById('offTempDate');
                offTempDate.value=data.tempDataProg.offDate;
                return;
            }

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

            const desiredTemperatureInput=document.getElementById('desiredTemperatureInput');
            if(desiredTemperatureInput.value!=data.desiredTemperature && !desiredTemperatureInput.matches(':focus')){
                desiredTemperatureInput.value=data.desiredTemperature;
            }

            const tempMode=document.getElementById('tempModeSelect');
            if(tempMode.value!=String.fromCharCode(data.mode)){
                tempMode.value=String.fromCharCode(data.mode);
            }
            
        })
        .catch(error => console.error("Error al obtener la temperatura actual:", error));
}

function tempOff() {
    tempData.status = modes.off;
    tempData.mode = document.getElementById("tempModeSelect").value
    tempData.desiredTemperature = document.getElementById("desiredTemperatureInput").value

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
    tempData.mode = document.getElementById("tempModeSelect").value
    tempData.desiredTemperature = document.getElementById("desiredTemperatureInput").value
    
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

function updateTempData() {
    tempData.mode=document.getElementById("tempModeSelect").value;
    tempData.status=modes.on; //Para actualzar los datos de la temperetura, debe de estar prendido

    if(tempData.mode===modes.autoMode){ //Si es modo automatico, agarrar el valor del input
        tempData.desiredTemperature=document.getElementById("desiredTemperatureInput").value
    }
    
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

function saveTempProg() {
    tempData.tempDataProg.mode = document.getElementById("tempProgModeSelect").value
    tempData.tempDataProg.desiredTemperature = document.getElementById("desiredProgTemperatureInput").value
    tempData.tempDataProg.offDate = document.getElementById("offTempDate").value
    tempData.tempDataProg.onDate = document.getElementById("onTempDate").value
    
    fetch("http://192.168.31.191/api/TempProg/Update", {
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



//Obtendra la temperatura cada 2s
if (window.location.pathname === "/") {
    setInterval(getTempData, 2000);
}