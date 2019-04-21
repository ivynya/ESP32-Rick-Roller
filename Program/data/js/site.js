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
                   "Contacting welcoming party"];

function cycleFlavorText(element, interval, repeats, callback) {
  const node = document.querySelector(element);
  const progBar = document.querySelector(".ldBar").ldBar;
  var cycle = 0;
  var timer = setInterval(function() {
    animateCSS(element, "fadeOutDown", false, function() {
      node.innerHTML = flavorTexts[cycle-1];
      animateCSS(element, "fadeInDown", true, "");
      progBar.set((cycle / repeats) * 100);
    });
    cycle += 1;
  }, interval + 1000);

  setTimeout(function() {
    clearInterval(timer);
    if (typeof callback === "function") callback()
  }, (repeats + 3) * interval);
}

document.addEventListener("DOMContentLoaded", function(event) {
  var cycleInterval = 2000;
  var cycles = 3;

  animateCSS(".loading", "fadeIn", true,
    setTimeout(function() {
      animateCSS("#loading-text", "fadeIn", true, function() {
        cycleFlavorText("#loading-text", cycleInterval, cycles, function() {
          animateCSS(".loading", "fadeOut", true, function() {
            animateCSS("#rick", "fadeIn", true, null);
            var rick = document.querySelector("#rick");
            rick.play();
            setTimeout(function() {
              animateCSS("#message", "fadeIn", true, null)
              animateCSS("#notification", "fadeInUp", true, null)
              animateCSS(".contentBlock", "fadeIn", true, null)
            }, 1000);
  })})})}, 1000));
  // show content (not implemented)
});
