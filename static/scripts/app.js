const API = 'https://make.hmc.edu/api/v1';


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

        enableRenderButtons();
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

    if (dpi < 50) {
        dpi += " <span class='red'>(WARNING - Low DPI)</span>";
    }

    info.innerHTML = `Size: ${width}x${height} inches<br>DPI: ${dpi}`;
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

function openPrintConfirmation() {
    // Open confirmation modal
    document.getElementById("id-input").getElementsByTagName("input")[0].value = "";
    document.getElementById("print-confirmation-container").classList.remove("hidden");
}

function closePrintConfirmation() {
    // Close confirmation modal
    document.getElementById("print-confirmation-container").classList.add("hidden");
}

async function printImage() {
    // Print the image
    let options = getOptions();
    options["print"] = true;

    // Log print
    await logPrint(options);

    await requestNewRender(options);

    closePrintConfirmation();
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

async function checkID() {
    // Prompt user for ID number
    let id_number = document.getElementById("id-input").getElementsByTagName("input")[0].value;

    id_number = parseCollegeID(id_number);

    if (id_number == null) {
        return "Error: Invalid ID number";
    }

    setAbleToPrint(true);
    return;
    
    // Call api to verify ID
    const response = await fetch(`${API}/users/info/${id_number}`);

    if (response.status == 200) {
        // Get data
        const data = await response.json();

        console.log(data);

        // If data is null, return false
        if (data == null) {
            return "Error: User not in system";
        }

        // If data.passed_quizzes does not contain
        // "General", return error
        if (!data.passed_quizzes.includes("General")) {
            setAbleToPrint(false);
            return "Error: User has not passed General Safety Quiz";
        }

        state.user_data = data;
        state.college_id = id_number;

        setAbleToPrint(true);
        return "Success";
    } else {
        setAbleToPrint(false);
        return "Error: User not in system";
    }
}

function setAbleToPrint(able) {
    if (able) {
        document.getElementById("id-input").getElementsByTagName("input")[0].classList.remove("error");
        document.getElementById("print-confirmation-yes").disabled = false;
    } else {
        document.getElementById("id-input").getElementsByTagName("input")[0].classList.add("error");
        document.getElementById("print-confirmation-yes").disabled = true;
    }
}

function parseCollegeID(collegeID) {
    collegeID = collegeID.trim();

    if (collegeID.length == 0) {
        return null;
    }

    if (collegeID.includes("-") || collegeID.includes("_") || collegeID.includes(" ")) {
        collegeID = collegeID.replace(/[_ ]/g, "-");

        return parseInt(collegeID.split("-")[0]);
    } else {
        return parseInt(collegeID);
    }
}

function openLog() {
    const log_el = document.getElementById("log-container");
    const log_content_el = document.getElementById("log-content");

    // Loop through all items in indexedDB using localforage
    let rows = [];
    localforage.iterate((value, key, iterationNumber) => {
        console.log([key, value]);

        // Create table row
        const tr = document.createElement("tr");

        // Create table data
        const td1 = document.createElement("td");
        const td2 = document.createElement("td");
        const td3 = document.createElement("td");
        const td4 = document.createElement("td");
        const td5 = document.createElement("td");

        // Set inner HTML
        td1.innerHTML = new Date(value.timestamp).toLocaleString();
        td2.innerHTML = value.user_info.college_id
        td3.innerHTML = value.user_info.name;
        td4.innerHTML = value.user_info.college_email;
        td5.innerHTML = value.options.paper_width + " inches";

        // Append to table row
        tr.appendChild(td1);
        tr.appendChild(td2);
        tr.appendChild(td3);
        tr.appendChild(td4);
        tr.appendChild(td5);

        rows = [tr, ...rows];        
    }).then(() => {
        // Clear log content
        log_content_el.innerHTML = `
        <tr>
            <th>Timestamp</th>
            <th>College ID</th>
            <th>Name</th>
            <th>Email</th>
            <th>Roll Width</th>
        </tr>
        `;

        // Append rows to log content
        rows.forEach(row => {
            log_content_el.appendChild(row);
        });

        log_el.classList.remove("hidden");
    });
}

function closeLog() {
    document.getElementById("log-container").classList.add("hidden");
}

document.addEventListener("keydown", function (event) {
    if (event.key == "Enter") {
        renderPreview();
    }
});