/*
  @library      jQuery Hotkeys Plugin
  @version      0.8
  @author       John Resig
  @copyright    Copyright (c) 2010
  @website      https://github.com/tzuryby/jquery.hotkeys
  
  Dual licensed under the MIT or GPL Version 2 licenses.
  
  Based upon the plugin by Tzury Bar Yochay:
  
  Original idea by:
  Binny V A, http://www.openjs.com/scripts/events/keyboard_shortcuts/
*/
(function(c){c.hotkeys={version:"0.8",specialKeys:{8:"backspace",9:"tab",13:"return",16:"shift",17:"ctrl",18:"alt",19:"pause",20:"capslock",27:"esc",32:"space",33:"pageup",34:"pagedown",35:"end",36:"home",37:"left",38:"up",39:"right",40:"down",45:"insert",46:"del",96:"0",97:"1",98:"2",99:"3",100:"4",101:"5",102:"6",103:"7",104:"8",105:"9",106:"*",107:"+",109:"-",110:".",111:"/",112:"f1",113:"f2",114:"f3",115:"f4",116:"f5",117:"f6",118:"f7",119:"f8",120:"f9",121:"f10",122:"f11",123:"f12",144:"numlock",145:"scroll",191:"/",224:"meta"},shiftNums:{"`":"~","1":"!","2":"@","3":"#","4":"$","5":"%","6":"^","7":"&","8":"*","9":"(","0":")","-":"_","=":"+",";":": ","'":"\"",",":"<",".":">","/":"?","\\":"|"}};function j(f){if(typeof f.data!=="string"){return}var k=f.handler,i=f.data.toLowerCase().split(" ");f.handler=function(a){if(this!==a.target&&(/textarea|select/i.test(a.target.nodeName)||a.target.type==="text")){return}var d=a.type!=="keypress"&&c.hotkeys.specialKeys[a.which],g=String.fromCharCode(a.which).toLowerCase(),m,b="",e={};if(a.altKey&&d!=="alt"){b+="alt+"}if(a.ctrlKey&&d!=="ctrl"){b+="ctrl+"}if(a.metaKey&&!a.ctrlKey&&d!=="meta"){b+="meta+"}if(a.shiftKey&&d!=="shift"){b+="shift+"}if(d){e[b+d]=true}else{e[b+g]=true;e[b+c.hotkeys.shiftNums[g]]=true;if(b==="shift+"){e[c.hotkeys.shiftNums[g]]=true}}for(var h=0,l=i.length;h<l;h++){if(e[i[h]]){return k.apply(this,arguments)}}}}c.each(["keydown","keyup","keypress"],function(){c.event.special[this]={add:j}})})(jQuery);
