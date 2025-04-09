function toggleAutoModeTemp() {
    const checkbox = document.getElementById("toggleAutoMode");
    const tempAutoInput = document.getElementById("tempAutoInput");
    const tempHotButton = document.getElementById("tempHotButton");
    const tempColdButton = document.getElementById("tempColdButton");
    tempAutoInput.disabled = !checkbox.checked;
    tempHotButton.disabled = checkbox.checked;
    tempColdButton.disabled = checkbox.checked;
}

function toggleAutoModeAccs() {
    const checkbox = document.getElementById("toggleCreateKey");
    const accsKeyInput = document.getElementById("accsKeyInput");
    const createKetButton = document.getElementById("accsKeyButton");
    accsKeyInput.disabled = !checkbox.checked;
    createKetButton.disabled = !checkbox.checked;
}

function toggleSatateAccs() {
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
    const accsData = {
        createKey: false,
        key: "",
        mode: "",
        keys: [],
        status: "",
        date: ""
    };
    if (!checkbox.checked) {
        accsData.mode="N";
        // Enviar la petición GET con un header personalizado
        fetch("http://192.168.31.191/Accs/Off", {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(accsData)
        })
            .then(response => response.json())
            .then(data => console.log(data))
            .catch(error => console.error('Error:', error));
    }else{
        accsData.mode="E";
        // Enviar la petición GET con un header personalizado
        fetch("http://192.168.31.191/Accs/On", {
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
}

function updateTemperature(temp, mode) {
    console.log(temp);
    console.log(mode);
}

function sendAccsData(action) {

    const keyValue = document.getElementById("accsKeyInput").value;

    const accsData = {
        createKey: false,
        key: keyValue,
        mode: action,
        keys: [],
        status: "",
        date: ""
    };

    switch (action) {
        case 'K':
            accsData.createKey = true;
            accsData.status = "W";
            // Enviar la petición GET con un header personalizado
            fetch("http://192.168.31.191/Accs/CreateKey", {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(accsData)
            })
                .then(response => response.json())
                .then(data => console.log(data))
                .catch(error => console.error('Error:', error));
            break;
        case 'O':
            // Enviar la petición GET con un header personalizado
            fetch("http://192.168.31.191/Accs/CreateKey", {
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
            break;
        case 'C':
            // Enviar la petición GET con un header personalizado
            fetch("http://192.168.31.191/Accs/CreateKey", {
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
            break;
        default:
            console.log("REQUEST INVALIDO!")
    }
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
