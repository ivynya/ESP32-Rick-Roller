// Function modified from https://github.com/daneden/animate.css
function animateCSS(element, animationName, isPermanent, callback) {
  const node = document.querySelector(element)
  node.classList.add("animated", animationName)

  function handleAnimationEnd() {
    if (!isPermanent)
      node.classList.remove("animated", animationName)
    node.removeEventListener("animationend", handleAnimationEnd)

    if (typeof callback === "function") callback()
  }

  node.addEventListener("animationend", handleAnimationEnd)
}

var flavorTexts = ["Registering your device",
                   "Activating your connection",
                   "Anonymizing your information"];

function cycleFlavorText(element, interval, repeats, callback) {
  const node = document.querySelector(element);
  var cycle = 0;
  var timer = setInterval(function() {
    animateCSS(element, "fadeOutDown", false, function() {
      node.innerHTML = flavorTexts[cycle-1];
      animateCSS(element, "fadeInDown", true, "");
    });
    cycle += 1;
  }, interval + 1000);

  setTimeout(function() {
    clearInterval(timer);
    if (typeof callback === "function") callback()
  }, (repeats + 3) * interval);
}

document.addEventListener("DOMContentLoaded", function(event) {
  animateCSS(".loading", "fadeIn", true,
    setTimeout(function() {
      animateCSS("#loading-text", "fadeIn", true, function() {
        cycleFlavorText("#loading-text", 2000, 3, function() {
          animateCSS(".loading", "fadeOut", true, function() {
            animateCSS("#rick", "fadeIn", true, "");
          });
        });
      });
    }, 500)
  );
  // show content (not implemented)
});
