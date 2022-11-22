var state = {
    image_url: null,
    history: {},
    file: null,
}

const image_area = {
    top_left: { x: 293, y: 405 },
    top_right: { x: 696, y: 405 },
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

    state.history = {};
    state.file = event.target.files[0];
    renderPreview();
}

function dropHandler(event) {
    document.getElementById("display").classList.remove("fileover");
    document.getElementById("display").classList.add("loading");

    event.preventDefault();
    state.history = {};
    state.file = event.dataTransfer.files[0];
    renderPreview();
}

async function requestNewRender(options, show=true) {
    console.log("Requesting new render");
    let formData = new FormData();

    formData.append("file", state.file);
    formData.append("options", JSON.stringify(options));

    // Request a new render
    let xhr = new XMLHttpRequest();
    xhr.open("POST", "/renderImage", true);
    xhr.send(formData);

    xhr.onload = function () {
        if (xhr.status == 200) {
            state.history[JSON.stringify(options)] = xhr.response;

            document.getElementById("display").classList.remove("loading");
            document.getElementById("display").classList.add("loaded");

            if (show) {
                state.image_url = xhr.response;
                showPreview(state.image_url, false);
            }
        }
    }
}

function renderPreview(options=false) {
    if (!options) {
        // Render the preview
        options = getOptions();
    }
    options["preview"] = true;

    if (state.history[JSON.stringify(options)]) {
        console.log("Using cached render");
        state.image_url = state.history[JSON.stringify(options)];
        showPreview(state.image_url);
    } else {
        requestNewRender(JSON.parse(JSON.stringify(options)), show=true);
        
        options.side = options.side === "short" ? "long" : "short";

        requestNewRender(options, show=false);
    }
}

function showPreviewTemp(side) {
    let options = getOptions();

    options["side"] = side;
    options["preview"] = true;

    let url = state.history[JSON.stringify(options)];

    if (url != undefined) {
        showPreview(url);
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

    if (state.image_url) {
        showPreview(state.image_url);
    }
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

    renderPreview();
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

    let options = getOptions();

    options["preview"] = true;

    let url = state.history[JSON.stringify(options)];

    if (url != undefined) {
        state.image_url = url;
        showPreview(state.image_url);
    }
}

function valueOfSelectedChildren(el) {
    for (let i = 0; i < el.children.length; i++) {
        if (el.children[i].classList.contains("selected")) {
            return el.children[i].value;
        }
    }
}

function getOptions() {
    // Get options from selected html elements
    let options = {
        image_area: image_area,
        side: null,
        max_size: false,
        specific_width: null,
        specific_height: null,
        specific_dpi: null,
        paper_width: null,
        print: false,
    };

    options.side = valueOfSelectedChildren(document.getElementById("side-select"));
    let type_of_sizing = valueOfSelectedChildren(document.getElementById("sizing-select"));

    switch (type_of_sizing) {
        case "max_size":
            options.max_size = true;
            break;
        case "specific_size":
            options.specific_width = document.getElementById("width-input").value;
            options.specific_width = options.specific_width == 0 ? null : Number(options.specific_width);

            options.specific_height = document.getElementById("height-input").value;
            options.specific_height = options.specific_height == 0 ? null : Number(options.specific_height);
            break;
        case "specific_dpi":
            options.specific_dpi = Number(document.getElementById("dpi-input").value);
            break;
    }

    options.paper_width = Number(valueOfSelectedChildren(document.getElementById("size-select")));

    return options;
}

function printImage() {
    // Print the image
    let options = getOptions();
    options["print"] = true;

    requestNewRender(options);
}

document.addEventListener("keydown", function (event) {
    if (event.key == "Enter") {
        renderPreview();
    }
});