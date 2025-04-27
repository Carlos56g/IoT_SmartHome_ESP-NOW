const accsData = {
    key: "",
    mode: "",
    keys: [],
    status: "",
    date: ""
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


function toggleCreateKeyAccs() {
    const checkbox = document.getElementById("toggleCreateKey");
    const accsKeyInput = document.getElementById("accsKeyInput");
    const createKetButton = document.getElementById("accsKeyButton");
    accsKeyInput.disabled = !checkbox.checked;
    createKetButton.disabled = !checkbox.checked;
}

function toggleOnOffAccs() {
    const checkbox = document.getElementById("OnOffAccs");
    const div = document.getElementById("Accs");
    const excludeIds = ["goToHistoryBtn", "goToKeysBtn", "OnOffAccs"];
    const elements = div.querySelectorAll("input, button");

    elements.forEach(el => {
        if (!excludeIds.includes(el.id))
            el.disabled = !checkbox.checked;
    })
    return checkbox.checked;
}

function OnOffAccs() {
    if (toggleOnOffAccs()) {
        accsOn();
        document.getElementById("Accs").className = "enabled-style";
    }
    else {
        accsOff();
        document.getElementById("Accs").className = "disabled-style";
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
                var historyDescription;
                switch (item.status) {
                    case 'J': // 'J'
                      historyDescription = "Aceptado";
                      break;
                    case 'K': // 'K'
                      historyDescription = "Denegado";
                      break;
                    case 'L': // 'L'
                      historyDescription = "Esperando nueva llave";
                      break;
                    case 'N': // 'N'
                      historyDescription = "Usuario registrado";
                      break;
                    case 'E': // 'E'
                      historyDescription = "Acceso abierto";
                      break;
                    case 'F': // 'F'
                      historyDescription = "Acceso cerrado";
                      break;
                    case 'G': // 'G'
                      historyDescription = "Apagado";
                      break;
                    case 'H': // 'H'
                      historyDescription = "Encendido";
                      break;
                    case 'I': // 'I'
                      historyDescription = "Creación de llave";
                      break;
                    case 'A': // 'A'
                      historyDescription = "Eliminación de llaves";
                      break;
                  }

                row.innerHTML = `
                <td>${item.key}</td>
                <td>${item.date}</td>
                <td>${historyDescription}</td>
                `;
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

function getAccsData() {
    fetch('/api/Accs/get')
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

if(window.location.pathname=='/Accs/Details'){
    setInterval(triggerAccsAction, 2000);
}

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
