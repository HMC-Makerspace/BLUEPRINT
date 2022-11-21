var state = {
    image: null,
}

const image_area = {
    top_left: { x: 293, y: 405 },
    top_right: { x: 696 , y: 405 },
    bottom_left: { x: 283, y: 668 },
    bottom_right: { x: 705, y: 668 }
}

function dragOverHandler(event) {
    event.preventDefault();

    // Change the style of the drop area
    document.getElementById("display").classList.add("fileover");
}

function dragLeaveHandler(event) {
    event.preventDefault();

    // Change the style of the drop area
    document.getElementById("display").classList.remove("fileover");
}

function openFile() {
    // Open file dialog to pick a image file
    document.getElementById("file-input").click();
}

function loadFile(event) {
    document.getElementById("display").classList.add("loading");

    sendFileToApi(event.target.files[0]);
}

function dropHandler(event) {
    document.getElementById("display").classList.remove("fileover");
    document.getElementById("display").classList.add("loading");

    event.preventDefault();

    sendFileToApi(event.dataTransfer.files[0]);
}

function sendFileToApi(file) {
    let options = getOptions();

    let formData = new FormData();
    
    formData.append("options", JSON.stringify(options));
    formData.append("file", file);

    let xhr = new XMLHttpRequest();
    xhr.open("POST", "/processFile", true);
    xhr.send(formData);

    xhr.onload = function () {
        if (xhr.status === 200) {
            return xhr.response;
        } else {
            alert("Error " + xhr.status + ": " + xhr.statusText);
        }
    }
}

function requestNewRender(options) {
    let formData = new FormData();

    formData.append("options", JSON.stringify(options));

    // Request a new render
    let xhr = new XMLHttpRequest();
    xhr.open("POST", "/renderImage", true);
    xhr.send(formData);

    xhr.onload = function () {
        if (xhr.status === 200) {
            return xhr.response;
        } else {
            alert("Error " + xhr.status + ": " + xhr.statusText);
        }
    }
}

function renderPreview() {
    // Render the preview
    let options = getOptions();

    options["preview"] = true;

    let response = requestNewRender(options);

    if (response) {
        state.image = response.image;
        showPreview(response.image);
    }    
}

function showPreview(image) {
    // Display the image
    const img = document.getElementById("preview");

    img.style.backgroundImage = `url(${image})`;
}

function clearPreview() {
    // Clear the preview
    const img = document.getElementById("preview");

    img.style.backgroundImage = "";
}

function setPaperSize(index) {
    const el = document.getElementById("size-select");

    for (let i = 0; i < el.children.length; i++) {
        if (i === index) {
            el.children[i].classList.add("selected");
        } else {
            el.children[i].classList.remove("selected");
        }
    }
}

function setSizing(index) {
    const el = document.getElementById("sizing-select");

    for (let i = 0; i < el.children.length; i++) {
        if (i === index) {
            el.children[i].classList.add("selected");
        } else {
            el.children[i].classList.remove("selected");
        }
    }

    switch (index) {
        case 0:
            document.getElementById("h-w-input").classList.add("hidden");
            document.getElementById("d-input").classList.add("hidden");
            break;
        case 1:
            document.getElementById("h-w-input").classList.remove("hidden");
            document.getElementById("d-input").classList.add("hidden");
            break;
        case 2:
            document.getElementById("h-w-input").classList.add("hidden");
            document.getElementById("d-input").classList.remove("hidden");
            break;
    }
}

function setDPI(value) {
    document.getElementById("dpi-input").value = Math.floor(Math.min(10000, Math.max(40, value)));
}

function setBoundedValue(el) {
    el.value = Math.floor(Math.min(el.max, Math.max(el.min, el.value)));
}

function setSide(index) {
    const el = document.getElementById("side-select");

    for (let i = 0; i < el.children.length; i++) {
        if (i === index) {
            el.children[i].classList.add("selected");
        } else {
            el.children[i].classList.remove("selected");
        }
    }
    
}

function getOptions() {
    // Get options from selected html elements
    let options = {
        image_area: image_area,
        max_size: null,
        specific_width: null,
        specific_height: null,
        specific_dpi: null,
        paper_width: null,
    };

    return options;
}