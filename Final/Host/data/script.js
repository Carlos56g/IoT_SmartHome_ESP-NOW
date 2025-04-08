function toggleInputState() {
    const checkbox = document.getElementById("toggleAutoMode");
    const tempAutoInput = document.getElementById("tempAutoInput");
    const tempHotButton = document.getElementById("tempHotButton");
    const tempColdButton = document.getElementById("tempColdButton");
    tempAutoInput.disabled = !checkbox.checked;
    tempHotButton.disabled = checkbox.checked;
    tempColdButton.disabled = checkbox.checked;
}

function updateTemperature(temp, mode){
    console.log(temp);
    console.log(mode);
}

function createLightInterface(){
    fetch("http://192.168.31.191/getLights") //Metodo Get que obtiene la informacion de luces del modulo
    .then(response => response.json()) //La respuesta la convierte en JSON
    .then (data => {
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
