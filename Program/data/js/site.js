var startTime, scrolledDown = 0;

// Function modified from https://github.com/daneden/animate.css
function animateCSS(element, animationName, isPermanent, callback) {
  const node = document.querySelector(element);
  node.classList.add("animated", animationName);

  function handleAnimationEnd() {
    if (!isPermanent)
      node.classList.remove("animated", animationName);
    node.removeEventListener("animationend", handleAnimationEnd);

    if (typeof callback === "function") callback();
  }

  node.addEventListener("animationend", handleAnimationEnd);
}

document.addEventListener("DOMContentLoaded", function(event) {
  animateCSS("#rick", "fadeIn", true, null);
  setTimeout(function() {
    animateCSS("#message", "fadeIn", true, null);
    animateCSS("#notification", "fadeInUp", true, null);
    var content = document.getElementById("contentBlock");
    content.style.display = "block";
    animateCSS("#contentBlock", "fadeIn", true, null);
  }, 1000);

  startTime = new Date();
});

// Get stats from server
function loadStatistics() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var responseJSON = JSON.parse(this.responseText);
      document.getElementById("visitors").innerHTML = responseJSON.v;
      document.getElementById("avgtime").innerHTML = responseJSON.s + " seconds";
      document.getElementById("viewed").innerHTML = responseJSON.i;
    }
  };
  xhttp.open("GET", "persistent.txt", true);
  xhttp.send();
}

window.addEventListener("scroll", function() {
  var contentBlock = document.getElementById("contentBlock");
  if (document.body.scrollTop > contentBlock.getBoundingClientRect().top) {
    scrolledDown = 1;
  }
});

// On browser close, send stats data
window.addEventListener("unload", logData, false);
function logData() {
  var data = new FormData();
  // Seconds spent on site
  var endTime = new Date();
  data.set("s", Math.round((endTime - startTime) / 1000));
  // If user scrolled to see info (0 = false, 1 = true)
  data.set("v", scrolledDown);
  navigator.sendBeacon("/addvisit", data);
}
