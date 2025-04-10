const accsData = {
    createKey: false,
    key: "",
    mode: "",
    keys: [],
    status: "",
    date: ""
};

function toggleAutoModeTemp() {
    const checkbox = document.getElementById("toggleAutoMode");
    const tempAutoInput = document.getElementById("tempAutoInput");
    const tempHotButton = document.getElementById("tempHotButton");
    const tempColdButton = document.getElementById("tempColdButton");
    tempAutoInput.disabled = !checkbox.checked;
    tempHotButton.disabled = checkbox.checked;
    tempColdButton.disabled = checkbox.checked;
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
    const toggleCreateKey = document.getElementById("toggleCreateKey");
    const accsKeyInput = document.getElementById("accsKeyInput");
    const createKetButton = document.getElementById("accsKeyButton");
    const accsOpenButton = document.getElementById("accsOpenButton");
    const accsCloseButton = document.getElementById("accsCloseButton");
    toggleCreateKey.disabled = !checkbox.checked;
    accsKeyInput.disabled = !checkbox.checked;
    createKetButton.disabled = !checkbox.checked;
    accsOpenButton.disabled = !checkbox.checked;
    accsCloseButton.disabled = !checkbox.checked;
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

function accsOff() {
    accsData.createKey = false;
    accsData.mode = "N";
    // Enviar la petición GET con un header personalizado
    fetch("http://192.168.31.191/api/Accs/OnOff", {
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
    accsData.createKey = false;
    accsData.mode = "E";
    // Enviar la petición GET con un header personalizado
    fetch("http://192.168.31.191/api/Accs/OnOff", {
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
    accsData.createKey = true;
    accsData.mode = "K";
    accsData.key = document.getElementById("accsKeyInput").value;
    // Enviar la petición GET con un header personalizado
    fetch("http://192.168.31.191/api/Accs/CreateKey", {
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
    accsData.createKey = false;
    accsData.mode = "O";
    fetch("http://192.168.31.191/api/Accs/OpenClose", {
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
    accsData.createKey = false;
    accsData.mode = "C";
    fetch("http://192.168.31.191/api/Accs/OpenClose", {
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


function createLightInterface() {
    fetch("http://192.168.31.191/getLights") //Metodo Get que obtiene la informacion de luces del modulo
        .then(response => response.json()) //La respuesta la convierte en JSON
        .then(data => {
            const lightsInterface = document.getElementById("lightsInterface");
            lightsInterface.innerHTML = '';

            data.forEach(element => {
                const individualLight = document.createElement("div");
                div.textContent = element;
                lightsInterface.appendChild(individualLight);
            });
        })

        .catch(error => console.log('Error al obtener datos:', error));
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
      alert(data);  // Muestra la respuesta, en este caso "Historial eliminado!"
    })
    .catch(error => {
      console.error("Error al eliminar el historial:", error);
      alert("Hubo un problema al eliminar el historial.");
    });
  }


function deleteAccsHistory() {
  fetch('/api/Accs/History/delete', {
    method: 'GET',  // Usamos el método GET para llamar al servidor
  })
  .then(response => response.text())  // Respuesta en formato de texto
  .then(data => {
    console.log("Respuesta del servidor:", data);
    alert(data);  // Muestra la respuesta, en este caso "Historial eliminado!"
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
      alert(data);  // Muestra la respuesta, en este caso "Historial eliminado!"
    })
    .catch(error => {
      console.error("Error al eliminar las llaves:", error);
      alert("Hubo un problema al eliminar las llaves.");
    });
  }
  
