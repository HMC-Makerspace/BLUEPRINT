const API = 'https://make.hmc.edu/api/v1';

const price_per_sq_ft = 0.60;
const price_per_sq_in = price_per_sq_ft / (12.0 * 12.0);

var state = {
    image_obj: null,
    history: {},
    file: null,
    isPDF: false,
    paper_width: 36,
    college_id: null,
    user_data: null,
}

const image_area = {
    top_left: { x: 293, y: 405 },
    top_right: { x: 696, y: 405 },
    bottom_left: { x: 283, y: 668 },
    bottom_right: { x: 705, y: 668 }
}

var loading_timeout = null;

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
    // If the file name starts with ._ (macOS hidden file), return
    if (event.target.files[0].name.startsWith("._")) {
        alert("Error: Invalid file");
        return;
    }

    state.history = {};
    state.file = event.target.files[0];

    // if it's a pdf
    if (state.file.type == "application/pdf") {
        state.isPDF = true;
        // Disable dpi button
        document.getElementById("specific_dpi").disabled = true;
    } else {
        state.isPDF = false;
        // Enable dpi button
        document.getElementById("specific_dpi").disabled = false;
    }

    renderPreview();
}

function dropHandler(event) {
    document.getElementById("display").classList.remove("fileover");

    event.preventDefault();
    state.history = {};
    state.file = event.dataTransfer.files[0];

    // if it's a pdf
    if (state.file.type == "application/pdf") {
        state.isPDF = true;
        // Disable dpi button
        document.getElementById("specific_dpi").disabled = true;
    } else {
        state.isPDF = false;
        // Enable dpi button
        document.getElementById("specific_dpi").disabled = false;
    }

    renderPreview();
}

async function requestNewRender(options, show=true) {
    console.log("Requesting new render");
    disableRenderButtons();
    let formData = new FormData();

    formData.append("file", state.file);
    formData.append("options", JSON.stringify(options));

    // Request a new render
    let xhr = new XMLHttpRequest();
    xhr.open("POST", "/renderImage", true);
    xhr.send(formData);

    xhr.onload = function () {
        if (xhr.status == 200) {
            state.history[JSON.stringify(options)] = JSON.parse(xhr.response);

            document.getElementById("display").classList.add("loaded");

            clearTimeout(loading_timeout);
            document.getElementById("image-loading-container").classList.add("hidden");

            if (show) {
                state.image_obj = JSON.parse(xhr.response);
                enableRenderButtons();

                showPreview(state.image_obj, false);
            }
        } else {
            console.error("Error: " + xhr.status);

            clearTimeout(loading_timeout);
            document.getElementById("image-loading-container").classList.add("hidden");

            if (xhr.status == 415) {
                alert("Error: File type not supported. Please upload a PDF, SVG, or supported image file.");
            } else {
                alert("Error: " + xhr.status);
            }
        }
    }
}

async function renderPreview(options=false) {
    loading_timeout = setTimeout(function () {
        document.getElementById("image-loading-container").classList.remove("hidden");
    }, 100);

    if (!options) {
        // Render the preview
        options = getOptions();
    }
    options["preview"] = true;

    if (state.history[JSON.stringify(options)]) {
        console.log("Using cached render");
        state.image_obj = state.history[JSON.stringify(options)];
        showPreview(state.image_obj);
        // Stop loading_timeout
        clearTimeout(loading_timeout);
        document.getElementById("image-loading-container").classList.add("hidden");
    } else {
        await requestNewRender(JSON.parse(JSON.stringify(options)), show=true);
    }
}

function updateInfoBox() {
    let info = document.getElementById("info-box");

    let width = state.image_obj.width;
    let height = state.image_obj.height;
    let dpi = state.image_obj.dpi;

    // Ceiling to the nearest cent
    let price = Math.ceil(state.paper_width * state.image_obj.height * price_per_sq_in * 100) / 100;

    if (dpi < 50) {
        dpi += " <span class='red'>(WARNING - Low DPI)</span>";
    }

    if (width <= 5 || height <= 5) {
        document.getElementById("print").disabled = true;
        info.innerHTML = `Size: ${width}x${height} inches<br>DPI: ${dpi}<br>Price: $${price.toFixed(2)}<br><span class='red'>Image is too small to print</span>`;
    } else {
        document.getElementById("print").disabled = false;
        info.innerHTML = `Size: ${width}x${height} inches<br>DPI: ${dpi}<br>Price: $${price.toFixed(2)}`;
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
    updateInfoBox();
    img.style.backgroundImage = `url(${image.image_url})`;
}

function clearPreview() {
    // Clear the preview
    const img = document.getElementById("preview");

    img.style.backgroundImage = "";

    if (state.image_obj) {
        showPreview(state.image_obj);
    }
}

function triggerChange() {
    renderPreview();
}

function disableRenderButtons() {
    document.getElementById("print").disabled = true;
}

function enableRenderButtons() {
    document.getElementById("print").disabled = false;
}

function disableDPIButton() {
    document.getElementById("specific_dpi").disabled = true;
}

function enableDPIButton() {
    document.getElementById("specific_dpi").disabled = false;
}

function setPaperSize(index) {
    const el = document.getElementById("size-select");

    for (let i = 0; i < el.children.length; i++) {
        if (i === index) {
            el.children[i].classList.add("selected");
            state.paper_width = Number(el.children[i].value);
        } else {
            el.children[i].classList.remove("selected");
        }
    }

    triggerChange();
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

    triggerChange();
}

function setDPI(value) {
    document.getElementById("dpi-input").value = Math.floor(Math.min(10000, Math.max(40, value)));

    triggerChange();
}

function setBoundedValue(el, override_max=false, override_min=false) {
    let max = override_max ? override_max : el.max;
    let min = override_min ? override_min : el.min;

    console.log(max, min);

    el.value = Math.floor(Math.min(max, Math.max(min, el.value)));
    triggerChange();
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
    triggerChange();
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

async function printImage() {
    // Print the image
    let options = getOptions();
    options["print"] = true;

    // Log print
    await logPrint(options);

    await requestNewRender(options);
    openLoadingModal();
}

function closeGif() {
    // Close loading modal
    document.getElementById("gif-container").classList.add("hidden");
}

function openLoadingModal() {
    // Open loading modal
    document.getElementById("gif-container").classList.remove("hidden");
}

async function logPrint(options) {
    // Log print
    const log_data = {
        id_number: state.college_id ?? "Unknown",
        user_info: state.user_data ?? "Unknown",
        options: options,
        timestamp: Date.now(),
    };

    // Store log in indexedDB using localforage by timestamp
    await localforage.setItem(log_data.timestamp, log_data);
}

document.addEventListener("keydown", function (event) {
    if (event.key == "Enter") {
        renderPreview();
    }
});