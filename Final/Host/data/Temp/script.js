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

function toggleOnOffTemp() {
    const checkbox = document.getElementById("OnOffTemp");
    return checkbox.checked;
}

function OnOffTemp() {
    if (toggleOnOffTemp()) {
        tempOn();
        document.getElementById("Temp").className = "enabled-style";
    }
    else {
        tempOff();
        document.getElementById("Temp").className = "disabled-style";
    }
}

function getTempData() {
    updateInput = true;
    fetch('/api/Temp/get')
        .then(response => response.json())
        .then(data => {

            if (window.location.pathname === "/Temp/Schedule") {
                const desiredProgTemperatureInput = document.getElementById('desiredProgTemperatureInput');
                desiredProgTemperatureInput.value = data.tempDataProg.desiredTemperature;
                const tempProgModeSelect = document.getElementById('tempProgModeSelect');
                tempProgModeSelect.value = String.fromCharCode(data.tempDataProg.mode);
                const onTempDate = document.getElementById('onTempDate');
                onTempDate.value = data.tempDataProg.onDate;
                const offTempDate = document.getElementById('offTempDate');
                offTempDate.value = data.tempDataProg.offDate;
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

            const desiredTemperatureInput = document.getElementById('desiredTemperatureInput');
            if (desiredTemperatureInput.value != data.desiredTemperature && !desiredTemperatureInput.matches(':focus')) {
                desiredTemperatureInput.value = data.desiredTemperature;
            }

            const tempMode = document.getElementById('tempModeSelect');
            if (tempMode.value != String.fromCharCode(data.mode)) {
                tempMode.value = String.fromCharCode(data.mode);
            }

            if (data.mode != 79) {
                document.getElementById("desiredTemperatureInput").disabled = true;
            }
        })
        .catch(error => console.error("Error al obtener la temperatura actual:", error));
}


function tempOff() {
    tempData.status = modes.off;
    tempData.mode = document.getElementById("tempModeSelect").value
    tempData.desiredTemperature = document.getElementById("desiredTemperatureInput").value

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
    tempData.mode = document.getElementById("tempModeSelect").value
    tempData.desiredTemperature = document.getElementById("desiredTemperatureInput").value

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

function updateTempData() {
    tempData.mode = document.getElementById("tempModeSelect").value;
    tempData.status = modes.on; //Para actualzar los datos de la temperetura, debe de estar prendido

    if (tempData.mode === modes.autoMode) {
        document.getElementById('desiredTemperatureInput').disabled = false;
    } else {
        document.getElementById("desiredTemperatureInput").disabled = true;
    }

    if (tempData.mode === modes.autoMode) { //Si es modo automatico, agarrar el valor del input
        tempData.desiredTemperature = document.getElementById("desiredTemperatureInput").value
    }

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

function saveTempProg() {
    tempData.tempDataProg.mode = document.getElementById("tempProgModeSelect").value
    tempData.tempDataProg.desiredTemperature = document.getElementById("desiredProgTemperatureInput").value
    tempData.tempDataProg.offDate = document.getElementById("offTempDate").value
    tempData.tempDataProg.onDate = document.getElementById("onTempDate").value
    const error = document.getElementById('alert');
    if (validateDate(tempData.tempDataProg.onDate, tempData.tempDataProg.offDate)) {
        fetch("/api/TempProg/Update", {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(tempData)
        })
            .then(response => response.json())
            .then(data => console.log(data))
            .catch(error => console.error('Error:', error));
        error.textContent = "";
    }
    else {
        error.textContent = "Fecha Invalida";
    }
}

function validateDate(onDate, OffDate) {
    const onDateD = new Date(onDate);
    const OffDateD = new Date(OffDate);

    if (onDateD > OffDateD)
        return false;
    else
        return true;
}

function validateMode() {
    var mode = document.getElementById("tempProgModeSelect").value;
    if (mode != modes.autoMode) {
        document.getElementById("desiredProgTemperatureInput").disabled = true;
    }

}


//Obtendra la temperatura cada 2s
if (window.location.pathname === "/Temp/Details") {
    setInterval(getTempData, 2000);
}


