/*
  WebFont Demo javascript
  
  (C) 2013-2015 High-Logic B.V.
  
  http://www.high-logic.com
*/

var haveErrors = false;
var haveOTLF   = false;

function Initialize() {
  if (fctrial) {
    AddError("Because this Web Font is created with a trial version of FontCreator, only a subset of characters is available. See the FontCreator User Manual about the limitations of exported fonts during the trial period.");
  }
  if (symbolfont) {
    AddError("符号字体为旧版的 Windows 字体. 它们可能不支持其它的操作系统, 并且字缩进和拼写检查功能会不工作. Unicode 字体包含有符号字符, 因此我们推荐你将符号字体转换为 Unicode 字体, 除非你想让它支持旧的软件. 转到 FontCreator 的主菜单然后选择工具 -> 转换字体 -> 转换为 Unicode 字体.");
  }
  BrowserCheck(otf.length > 0, colorfont);
  ProcessErrors();
  InitializeOpenTypeFeatures();
  UpdatePreviewText(document.getElementById("input"));
}

function AddError(aError) {
  haveErrors = true;
  id = document.getElementById('errorlist');
  id.innerHTML += "<li>" + aError + "</li>";
  return false;
}

function ProcessErrors() {
  if (!haveErrors) {
    document.getElementById('errors').style.display = "none";
  }
  else {
    document.getElementById("nojserror").style.display = "none";
  }
}

function DetermineBrowser() {
  var browserstr = navigator.userAgent.toLowerCase();
  var result = Array();
  result["name"] = "unknown";
  result["fullname"] = "unknown";
  result["version"] = "unknown";
  if (match = browserstr.match("chrome/([0-9]+)")) {
    result["name"] = "chrome";
    result["fullname"] = "Google Chrome";
    result["version"] = match[1];
  }
  else if (match = browserstr.match("firefox/([0-9]+)")) {
    result["name"] = "firefox";
    result["fullname"] = "Mozilla Firefox";
    result["version"] = match[1];
  }
  else if (match = browserstr.match("opera/([0-9]+)")) {
    result["name"] = "opera";
    result["fullname"] = "Opera";
    result["version"] = match[1];
  }    
  else if (match = browserstr.match("safari/([0-9]+)")) {
    result["name"] = "safari";
    result["fullname"] = "Apple Safari";
    result["version"] = match[1];
  }
  else if (match = browserstr.match("msie ([0-9]+)")) {
    result["name"] = "ie";
    result["fullname"] = "Microsoft Internet Explorer";
    result["version"] = match[1];
  }  
  else if (match = browserstr.match("trident/([0-9]+).+; .*? rv:([0-9]+)")) {
    /* IE 11 and up */
    if (match[1] >= 7) {
      result["name"] = "ie";
      result["fullname"] = "Microsoft Internet Explorer";
      result["version"] = match[2];
    }
  }   
  return result;
}

function DetermineOS() {
  var browserstr = navigator.userAgent.toLowerCase();
  var result = Array();
  result["name"] = "unknown";
  result["version"] = "unknown";
  if (match = browserstr.match("windows.*? ([0-9\.]+);")) {
    result["name"] = "windows";
	result["version"] = match[1];
  }
  return result;
}

function BrowserCheck(aOTLF, aColor) {
 
  var browser = DetermineBrowser();
  var os = DetermineOS();
  var id = document.getElementById("errorlist");

  switch (browser["name"]) {
    case "firefox" :
      if (aOTLF) {
	    if (browser["version"] < 4) {
          return AddError("你需要至少版本为 FireFox 4 或兼容 CSS 3.0 的浏览器, 才可使用 OpenType Layout Features.");
        }
	  }
	  if (aColor) {
	    if (browser["version"] < 32) {
	      return AddError("你需要至少版本为 Firefox 32 的浏览器, 才能使用彩色字体.");
      }
	  }
    break;
    case "chrome" : 
      if (browser["version"] < 22) {
        return AddError("你需要至少版本为 Chrome 22 或兼容 CSS 3.0 的浏览器, 才可使用 OpenType Layout Features.");
      }
	  if (aColor) {
	    return AddError("Chrome 浏览器不支持彩色字体, 请提升版本至28.");
	  }
    break;
    case "ie" : 
      if (aOTLF && (browser["version"] < 10)) {
        return AddError("你需要至少版本为 Internet Explorer 10 或兼容 CSS 3.0 的浏览器, 才可使用 OpenType Layout Features.");
      }
	  if (aColor) {
		if (browser["version"] < 11) {
	      return AddError("你需要至少版本为 Internet Explorer 11 的浏览器, 才可使用彩色字体.");
		}
		else {
		  if (os["version"] < 6.3) {
		    return AddError("你需要至少版本为 Windows 8.1 的 Internet Explorer 才能使用彩色字体. 你也可以在32位全平台系统里使用 Firefox 来支持彩色字体, 二者可选其一.");
		  }
		}
	  }
    break;
    case "opera" : 
      if (browser["version"] < 15) {
        return AddError("你需要至少版本为 Opera 15 或兼容 CSS 3.0 的浏览器, 才可使用 OpenType Layout Features.");
      }
      else {
        return AddWarning("警告: 此版本的 Opera 浏览器可能不支持 OpenType Layout Features.");
      }
	  if (aColor) {
	    return AddError("Opera 浏览器不支持彩色字体, 请提升版本至16.");
	  }
    break;
    default :
      return AddError("无法检测你的浏览器和/或版本. OpenType Layout Features 将不能正确的显示.");
    break;
  }
}

function UpdateFeatures() {
  var featurestring = "";

  altval = document.getElementById("alternate").value;
  
  for (i = 0; i < otf.length; i++) {
    id = document.getElementById("otf_" + otf[i][0]);
    if (i > 0) {
      featurestring += ",";
    }
//    featurestring += "'" + otf[i][0] + "' " + (id.checked ? "1" : "0");
    featurestring += "'" + otf[i][0] + "' " + (id.checked ? altval : "0");
  }  
  id = document.getElementById("preview");
  // Firefox 4+
  id.style.MozFontFeatureSettings = featurestring;
  // Chrome 20+ / Safari (OSX only)
  id.style.webkitFontFeatureSettings = featurestring;
  // Official CSS 3.0
  id.style.fontFeatureSettings = featurestring;
}

function SetAllFeatures(aValue) {
  for (i = 0; i < otf.length; i++) {
    id = document.getElementById("otf_" + otf[i][0]);
    id.checked = aValue;
  }  
  UpdateFeatures();
}

function SetDefaultFeatures() {
  for (i = 0; i < otf.length; i++) {
    id = document.getElementById("otf_" + otf[i][0]);
    id.checked = otf[i][2];
  }  
  UpdateFeatures();
}

function InitializeOpenTypeFeatures() {
  id = document.getElementById("otfeatures");
  if (otf.length == 0) {
    id.innerHTML += "此字体不包含任何的 OpenType Layout Features 内容.";
    return;
  
  }
  id.innerHTML += "<input type='button' value='不激活所有' onclick='SetAllFeatures(false)'/> ";
  id.innerHTML += "<input type='button' value='激活所有' onclick='SetAllFeatures(true)'/> ";
  id.innerHTML += "<input type='button' value='默认' onclick='SetDefaultFeatures()'/><hr/>";
  for (i = 0; i < otf.length; i++) {
    checked = otf[i][2] ? " checked='checked'" : "";
    id.innerHTML += "<label><input id='otf_"+otf[i][0]+"'"+checked+" type='checkbox' onclick='UpdateFeatures()'> " + otf[i][0] + " - " + otf[i][1] + "</label><br/>";
  }
  UpdateFeatures();
}

function UpdatePreviewText(sender) {
  id = document.getElementById("preview");
  id.innerHTML = sender.value.replace(/\n/g, "<br/>");
}

function UpdatePreviewQuickPick(sender) {
  id = document.getElementById("input");
  switch (sender.value) {
    case "0" : 
      id.value = id.defaultValue;
    break;
    case "1" : 
      id.value = "abcdefghijklmnopqrstuvwxyz\r\nABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n1234567890";
    break;
    case "2" : 
      id.value = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed molestie augue vel quam dapibus sollicitudin. Sed vestibulum erat ac nibh varius vestibulum. Quisque a quam at est sodales pulvinar ac vel nisl. Cras sit amet consectetur dolor. Aliquam nisl lorem, iaculis eget aliquam pharetra, tincidunt id turpis. Ut sit amet nisl sed ipsum tempor luctus. Aenean vitae tristique sem. Pellentesque turpis dolor, lacinia id fermentum et, dapibus ac ipsum. Etiam nisi quam, feugiat vitae eleifend vel, luctus et mi.";
    break;
  }
  UpdatePreviewText(document.getElementById("input"));
}

function UpdateFontSize(sender) {
  id = document.getElementById("preview");
  id.style.fontSize = sender.value + "pt";
  id.style.lineHeight = sender.value + "pt";
}

function UpdateFontColorBG(sender) {
  id = document.getElementById("preview");
  id.style.background = "#" + sender.value;
}

function UpdateFontColorFG(sender) {
  id = document.getElementById("preview");
  id.style.color = "#" + sender.value;
}

function UpdateAlternate(sender) {
  UpdateFeatures();
}

function UpdateDirection(sender) {
  id = document.getElementById("preview");
  id.style.direction = sender.value;
}