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

function getLightsData() {
    fetch('/api/Lights/get')
        .then(response => response.json())
        .then(data => {
            const tbody = document.getElementById('lightsBody');
            tbody.innerHTML = "";
            data.lightDevices.forEach((light, index) => {
                const row = document.createElement('tr');
                row.innerHTML = `
            <td>${index + 1}</td>
            
            <td><select id="lightModeSelect-${index}">
                            <option value="H" ${light.mode === 72 ? 'selected' : ''}>Encendido</option>
                            <option value="G" ${light.mode === 71 ? 'selected' : ''}>Apagado</option>
                            <option value="T" ${light.mode === 84 ? 'selected' : ''}>Presencia</option>
                            <option value="O" ${light.mode === 79 ? 'selected' : ''}>Automatico</option>
                            <option value="V" ${light.mode === 86 ? 'selected' : ''}>Presencia y Auto</option>
            </select></td>
            
            <td><input type="datetime-local" id="onLightDate-${index}" name="inicio" value=${light.onDate}></td>
            
            <td><input type="datetime-local" id="offLightDate-${index}" name="inicio" value=${light.offDate}></td>

            <td><select id="lightModeProgSelect-${index}">
                            <option value="H" ${light.pMode === 72 ? 'selected' : ''}>Encendido</option>
                            <option value="G" ${light.pMode === 71 ? 'selected' : ''}>Apagado</option>
                            <option value="T" ${light.pMode === 84 ? 'selected' : ''}>Presencia</option>
                            <option value="O" ${light.pMode === 79 ? 'selected' : ''}>Automatico</option>
                            <option value="V" ${light.pMode === 86 ? 'selected' : ''}>Presencia y Auto</option>
            </select></td>

            <td><input type="number" id="lightdesiredBrightness-${index}" name="inicio" value=${light.desiredBrightness}></td>
            
            <td><input type="number" id="lightdefaultTimeOn-${index}" name="inicio" value=${light.defaultTimeOn}></td>`
                    ;
                tbody.appendChild(row);
            });
        })
        .catch(error => console.error("Error al obtener la informacion de las luces:", error));
}

function updateLights() {
    const updatedLights = [];

    const rows = document.querySelectorAll("#lightsBody tr");
    rows.forEach((row, index) => {
        const light = {
            mode: document.getElementById(`lightModeSelect-${index}`).value,
            onDate: document.getElementById(`onLightDate-${index}`).value,
            offDate: document.getElementById(`offLightDate-${index}`).value,
            pMode: document.getElementById(`lightModeProgSelect-${index}`).value,
            desiredBrightness: parseFloat(document.getElementById(`lightdesiredBrightness-${index}`).value),
            defaultTimeOn: parseFloat(document.getElementById(`lightdefaultTimeOn-${index}`).value),
        };

        updatedLights.push(light);
    });

    const lightsData = {
        lightData: updatedLights
    };


    fetch("/api/Lights/Update", {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(lightsData)
    })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));


}


function lightOff() {
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

function OnOffLight() {
    if (toggleOnOffLight()) {
        lightOn();
        document.getElementById("Light").className = "enabled-style";
    }
    else {
        lightOff();
        document.getElementById("Light").className = "disabled-style";
    }
}

function toggleOnOffLight() {
    const checkbox = document.getElementById("OnOffLight");
    return checkbox.checked;
}



