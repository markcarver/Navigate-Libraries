<?php
/*
  @library     ICEY CSS Compressor
  @version     0.9
  @author      Robson
  @copyright   Copyright (C) 2008
  @website     http://iceyboard.no-ip.org/projects/css_compressor
  
  This class was modified (forked) for use with the Drupal module, Navigate.
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
   
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

class iceycss {
  
  private $file_props = array();
  private $file_selector = array();
 
  // functions appear in the order they are first called
  function compress($cssfile = NULL) {
     // make these variables available locally
     
     
     // check if no css was found
     if (!$cssfile) return FALSE;
     // remove html and css comments    
     $this->kill_comments($cssfile);   
     // trim whitespace from the start and end
     $cssfile = trim($cssfile);
     // turn all rgb values into hex
     $this->rgb2hex($cssfile);
     $this->long_hex_to_short_colours($cssfile);
     $this->remove_zero_measurements($cssfile);
     $this->sort_css($cssfile);
     $this->font_weight_text_to_numbers($cssfile);        
     $this->combine_identical_selectors();
     // $this->remove_overwritten_properties();
     for ($n = 0; $n < count($this->file_props); $n++) {
       // attempt to combine the different parts
       $this->combine_props_list($this->file_props[$n]);    
     }
     // for each rule in the file
     for ($n = 0; $n < count($this->file_props); $n++) {
       // run all the individual functions to reduce their size
       array_walk($this->file_props[$n], array($this, 'reduce_prop'));
     }
     // remove all the properties that were blanked out earlier
     $this->remove_empty_rules();
     // check if any rules are the same as other ones and remove the first ones
     $this->combine_identical_rules();
     // one final run through to remove all unnecessary parts of the arrays
     $this->remove_empty_rules();
     return $this->create_output();
  }
 
  // this removes html and css comments from the file
  function kill_comments(&$css) {
     // kill html comments
     $css = str_replace('<!--', '', $css);
     $css = str_replace('-->', '', $css);
     // kill css comments
     $css = preg_replace('/\/\*(.*?)\*\//si', '', $css);
  }
 
 
  function parseRGB($matches) {
    // set colour to nothing so it doesn't use the previous value
    $colour = '';
    // loop through each rgb value, for red, green and blue
    for ($n = 1; $n < 4; $n++) {
        // ff or 0f - always return two characters
        $colour .= strlen(dechex($matches[$n])) == 1 ? '0' . dechex($matches[$n]) : dechex($matches[$n]);
    }
    return '#' . $colour;
  }
  // converts any rgb values to hex values
  // i.e. rgb(255,170,0) -> #ffaa00
  function rgb2hex(&$string) {
     // loop only while there are rgb values in the string
     $string = preg_replace_callback("/rgb\((\d{1,3})\s*?,\s*?(\d{1,3})\s*?,\s*?(\d{1,3})\)/i", array($this, 'parseRGB'), $string);
  }
   
  // this converts hex colour codes to shorter text equivilants
  // only the standard sixteen colours codes are used here
  function long_hex_to_short_colours(&$string) {
     // the colours that are shorter than the hex representation of them
     $colours = array(
                 array('808080', 'gray'), array('008000', 'green'), array('800000', 'maroon'),
                 array('00080', 'navy'), array('808000', 'olive'), array('800080', 'purple'),
                 array('ff0000', 'red'), array('c0c0c0', 'silver'), array('008080', 'teal')
                     );
                   
     // loop through each colour
     for ($n = 0; $n < count($colours); $n++)
         // replace hex with colour name
         // ie #808080 -> gray
         $string = str_replace('#' . $colours[$n][0], $colours[$n][1], $string);
  }
  
  function colours_to_hex(&$item) {
    $colours = array(
      'aliceblue' => '#f0f8ff', 'antiquewhite' => '#faebd7', 'aqua' => '#00ffff', 'aquamarine' => '#7fffd4', 'azure' => '#f0ffff', 'beige' => '#f5f5dc', 'bisque' => '#ffe4c4',
      'black' => '#000000', 'blanchedalmond' => '#ffebcd', 'blue' => '#0000ff', 'blueviolet' => '#8a2be2', 'brown' => '#a52a2a', 'burlywood' => '#deb887', 'cadetblue' => '#5f9ea0',
      'chartreuse' => '#7fff00', 'chocolate' => '#d2691e', 'coral' => '#ff7f50', 'cornflowerblue' => '#6495ed', 'cornsilk' => '#fff8dc', 'crimson' => '#dc143c', 'cyan' => '#00ffff',
      'darkblue' => '#00008b', 'darkcyan' => '#008b8b', 'darkgoldenrod' => '#b8860b', 'darkgray' => '#a9a9a9', 'darkgreen' => '#006400', 'darkkhaki' => '#bdb76b', 'darkmagenta' => '#8b008b',
      'darkolivegreen' => '#556b2f', 'darkorange' => '#ff8c00', 'darkorchid' => '#9932cc', 'darkred' => '#8b0000', 'darksalmon' => '#e9967a', 'darkseagreen' => '#8fbc8f',
      'darkslateblue' => '#483d8b', 'darkslategray' => '#2f4f4f', 'darkturquoise' => '#00ced1', 'darkviolet' => '#9400d3', 'deeppink' => '#ff1493', 'deepskyblue' => '#00bfff',
      'dimgray' => '#696969', 'dodgerblue' => '#1e90ff', 'firebrick' => '#b22222', 'floralwhite' => '#fffaf0', 'forestgreen' => '#228b22', 'fuchsia' => '#ff00ff', 'gainsboro' => '#dcdcdc',
      'ghostwhite' => '#f8f8ff', 'gold' => '#ffd700', 'goldenrod' => '#daa520', 'gray' => '#808080', 'green' => '#008000', 'greenyellow' => '#adff2f', 'honeydew' => '#f0fff0',
      'hotpink' => '#ff69b4', 'indianred' => '#cd5c5c', 'indigo' => '#4b0082', 'ivory' => '#fffff0', 'khaki' => '#f0e68c', 'lavender' => '#e6e6fa', 'lavenderblush' => '#fff0f5',
      'lawngreen' => '#7cfc00', 'lemonchiffon' => '#fffacd', 'lightblue' => '#add8e6', 'lightcoral' => '#f08080', 'lightcyan' => '#e0ffff', 'lightgoldenrodyellow' => '#fafad2',
      'lightgreen' => '#90ee90', 'lightgrey' => '#d3d3d3', 'lightpink' => '#ffb6c1', 'lightsalmon' => '#ffa07a', 'lightseagreen' => '#20b2aa', 'lightskyblue' => '#87cefa',
      'lightslategray' => '#778899', 'lightsteelblue' => '#b0c4de', 'lightyellow' => '#ffffe0', 'lime' => '#00ff00', 'limegreen' => '#32cd32', 'linen' => '#faf0e6', 'magenta' => '#ff00ff',
      'maroon' => '#800000', 'mediumauqamarine' => '#66cdaa', 'mediumblue' => '#0000cd', 'mediumorchid' => '#ba55d3', 'mediumpurple' => '#9370d8', 'mediumseagreen' => '#3cb371',
      'mediumslateblue' => '#7b68ee', 'mediumspringgreen' => '#00fa9a', 'mediumturquoise' => '#48d1cc', 'mediumvioletred' => '#c71585', 'midnightblue' => '#191970', 'mintcream' => '#f5fffa',
      'mistyrose' => '#ffe4e1', 'moccasin' => '#ffe4b5', 'navajowhite' => '#ffdead', 'navy' => '#000080', 'oldlace' => '#fdf5e6', 'olive' => '#808000', 'olivedrab' => '#688e23',
      'orange' => '#ffa500', 'orangered' => '#ff4500', 'orchid' => '#da70d6', 'palegoldenrod' => '#eee8aa', 'palegreen' => '#98fb98', 'paleturquoise' => '#afeeee', 'palevioletred' => '#d87093',
      'papayawhip' => '#ffefd5', 'peachpuff' => '#ffdab9', 'peru' => '#cd853f', 'pink' => '#ffc0cb', 'plum' => '#dda0dd', 'powderblue' => '#b0e0e6', 'purple' => '#800080', 'red' => '#ff0000',
      'rosybrown' => '#bc8f8f', 'royalblue' => '#4169e1', 'saddlebrown' => '#8b4513', 'salmon' => '#fa8072', 'sandybrown' => '#f4a460', 'seagreen' => '#2e8b57', 'seashell' => '#fff5ee',
      'sienna' => '#a0522d', 'silver' => '#c0c0c0', 'skyblue' => '#87ceeb', 'slateblue' => '#6a5acd', 'slategray' => '#708090', 'snow' => '#fffafa', 'springgreen' => '#00ff7f',
      'steelblue' => '#4682b4', 'tan' => '#d2b48c', 'teal' => '#008080', 'thistle' => '#d8bfd8', 'tomato' => '#ff6347', 'turquoise' => '#40e0d0', 'violet' => '#ee82ee', 'wheat' => '#f5deb3',
      'white' => '#ffffff', 'whitesmoke' => '#f5f5f5', 'yellow' => '#ffff00', 'yellowgreen' => '#9acd32'
    );
    $item_parts = array();
    $pos = strpos($item, ':');
    $len = strlen($item);
    $item_parts[0] = substr($item, 0, $pos);
    $item_parts[1] = substr($item, ($pos + 1) - $len);
    // $item_parts = explode(':', $item);
    $properties = explode(' ', $item_parts[1]);
    foreach ($properties as $key => $property) {
      if (preg_match('/^url/', $property) || preg_match('/^!/', $property)) continue;
      // loop through each colour
      $properties[$key] = str_replace(array_keys($colours), array_values($colours), $property);    
    }
    if (implode(' ', $properties) != $item_parts[1])
      $item = $item_parts[0] . ':' . implode(' ', $properties);
  }
 
  // zero is always zero, so measurements don't matter
  function remove_zero_measurements(&$string) {
     // change 0 ems, 0 pixels and 0 percentages to 0
     // this wont change values like 10px, since those do need measurements
     $string = preg_replace('/\b0(pt|px|em)/', '0', $string);
  }
 
  // this seperates the css file into it's rules,
  // which it then sends to another function to sort into each part
  function sort_css($cssfile) {
     // the first thing to do is seperate everything out in the file
     // so loop round each rule in the file
     while ($cssfile)
     {
         // check if there is some more code
         if (substr_count($cssfile, '}'))
         {
             // the next rule is everything up to the squiggly bracket
             $rule = substr($cssfile, 0, strpos($cssfile, '}')+1);
             // seperate out everything in this rule and add it to different global arrays
             $this->parse_rules($rule);
             // remove that rule from the css file
             $cssfile = substr($cssfile, strlen($rule), strlen($cssfile));
         }    
         // no more rules?
         else
             // kill the css file variable to terminate this loop
             unset($cssfile);
     }
  }
 
  // turns font-weight text into numbers
  // for example: font-weight:normal -> font-weight:400
  function font_weight_text_to_numbers(&$string) {
     // make these variables available to this function
     for ($a = 0; $a < count($this->file_props); $a++)
     {
         for ($b = 0; $b < count($this->file_props[$a]); $b++)
         {
             if ($this->file_props[$a][$b] == 'font-weight:bold')
                 $this->file_props[$a][$b] = 'font-weight:700';
             if ($this->file_props[$a][$b] == 'font-weight:normal')
                 $this->file_props[$a][$b] = 'font-weight:400';
               
             $this->file_props[$a][$b] = str_replace('font:normal', 'font:400', $this->file_props[$a][$b]);
             $this->file_props[$a][$b] = str_replace('font:bold', 'font:700', $this->file_props[$a][$b]);    
         }
     }
  }
 
  // seperate the parts of each rule
  function parse_rules($css) {
     // make these variables available to this function
     
     
 
     // get the selectors contained in this part of the sheet
     $selector = substr($css, 0, strpos($css, '{'));
     // get the css properties and values contained in this part of the sheet
     $props = trim(substr($css, strlen($selector)+1, -1));
   
     // remove extra space from before, between and after the selector(s)
     $this->strip_space($selector);
     // remove any additional space
     $selector = trim(eregi_replace(', +', ',', $selector));    
     $selector = trim(eregi_replace(' +,', ',', $selector));    
     // seperate selector(s) and add to the global selector array    
     $this->file_selector[] = array_unique(explode(',', $selector));
   
     // shorten the css code
     $this->strip_space($props);
     // remove any additional space
     $props = trim(eregi_replace('(:|;) +', '\\1', $props));
     $props = trim(eregi_replace(' +(:|;)', '\\1', $props));
     $props = trim(eregi_replace('\( +', '(', $props));
     $props = trim(eregi_replace(' +\)', ')', $props));
     // if the last character is a semi-colon
     if (substr($props, strlen($props)-1, 1) == ';')
         // remove it
         $props = substr($props, 0, -1);
     // seperate properties and add to the global props array
     $this->file_props[] = explode(';', $props);
  }
 
  // removes white space from a string and returns the result
  function strip_space(&$string) {
     // kill whitespace on classes
     // kill new lines
     $string = str_replace(chr(10), '', $string);
     $string = str_replace(chr(13), '', $string);
     // change tabs into spaces
     $string = str_replace(chr(9), ' ', $string);
     // remove additional white space
     $string = eregi_replace(' +', ' ', $string);
  }
 
  // the following code combines rules with the same single selector
  // currently it only works on rules which have one selector
  function combine_identical_selectors() {
     // make these arrays available to this function
     
     
   
     // this will store which selectors have been used
     $cur_selectors = array();
   
     // loop through all the rules
     for ($a = 0; $a < count($this->file_selector); $a++)
     {
         // check there is only one selector
         if (count($this->file_selector[$a]) == 1)
         {
             // see if this selector has been used before
             if (in_array($this->file_selector[$a][0], $cur_selectors))
             {
                 // if so, loop round until it can be found
                 for ($b = 0; $b < count($cur_selectors); $b++)
                 {
                     // check if this matches a previous rule
                     if ($cur_selectors[$b] == $this->file_selector[$a][0])
                     {
                         // combine the properties in this rule and the previous one
                         // they remian in the order they were in the file, so new rules override old ones
                         $new_props = array_merge($this->file_props[$b], $this->file_props[$a]);
                         // replace the current props with all of them
                         $this->file_props[$a] = $new_props;
                         // kill the old selector
                         $this->file_selector[$b] = NULL;
                         // kill the old properties
                         $this->file_props[$b] = array(NULL);
                         // remove from the selectors array
                         $cur_selector[$b] = NULL;
                     }
                 }
             }
             // add the current selector to the selectors array
             $cur_selectors[] = $this->file_selector[$a][0];        
         }
         else
             // add nothing to maintain the selector index
             $cur_selectors[] = NULL;
     }
  }
 
  // this removes duplicates classes from rules
  // if a property is repeated the last one is only used, so old oens can be safely removed
  function remove_overwritten_properties() {
     // make these arrays available to this function
     
     
   
     // loop through rules
     for ($a = 0; $a < count($this->file_props); $a++)
     {
         // this will store the property list
         $cur_props = array();
         // loop through all the properties in the current rule
         for ($b = 0; $b < count($this->file_props[$a]); $b++)
         {
             // explode the property and the value
             $parts = array();
             $item = $this->file_props[$a][$b];
             $pos = strpos($item, ':');
             $len = strlen($item);
             $parts[0] = substr($item, 0, $pos);
             $parts[1] = substr($item, ($pos + 1) - $len);
             // $parts = explode(':', $this->file_props[$a][$b]);
             // check if this property has been used previously
             if (in_array($parts[0], $cur_props))
             {
                 // if so, find where
                 for ($c = 0; $c < count($cur_props); $c++)
                 {
                     // check if it's the same as the old one
                     if ($cur_props[$c] == $parts[0])
                         // kill the old one, it's not needed
                         $this->file_props[$a][$c] = NULL;
                 }
             }
             // add the type to the property array
             $cur_props[] = $parts[0];
         }
     }
  }
 
  // this is the list of properties that can be combined
  function combine_props_list(&$props) {
     // each call sends the current part of the stylesheet being worked on,
     // the combined property,
     // the parts which makes up this property
   
     $this->combine_props($props, 'padding', array('padding-top', 'padding-right', 'padding-bottom', 'padding-left'));    
   
     $this->combine_props($props, 'margin', array('margin-top', 'margin-right', 'margin-bottom', 'margin-left'));    
 
     $this->combine_props($props, 'list-style', array('list-style-type', 'list-style-position', 'list-style-image'));
     $this->combine_props($props, 'list-style', array('list-style-type', 'list-style-position'));
   
     $this->combine_props($props, 'outline', array('outline-color', 'outline-style', 'outline-width'));    
   
     // to do: this might be improvable
     $this->combine_props($props, 'background', array('background-color', 'background-image', 'background-repeat', 'background-attachment', 'background-position'));
 
     $this->combine_props($props, 'border-bottom', array('border-bottom-width', 'border-bottom-style', 'border-bottom-color'));
     $this->combine_props($props, 'border-top', array('border-top-width', 'border-top-style', 'border-top-color'));
     $this->combine_props($props, 'border-left', array('border-left-width', 'border-left-style', 'border-left-color'));
     $this->combine_props($props, 'border-right', array('border-right-width', 'border-right-style', 'border-right-color'));
 
     $this->combine_props_borders($props, 'border', array('border-bottom', 'border-top', 'border-left', 'border-right'));
     $this->combine_props($props, 'border', array('border-width', 'border-style', 'border-color'));
   
     // to do: this needs some checking
     $this->combine_props($props, 'font', array('font-style', 'font-variant', 'font-weight', 'font-size', 'line-height', 'font-family'));
     $this->combine_props($props, 'font', array('font-style', 'font-variant', 'font-weight', 'font-size', 'font-family'));
     $this->combine_props($props, 'font', array('font-variant', 'font-weight', 'font-size', 'line-height', 'font-family'));
     $this->combine_props($props, 'font', array('font-style', 'font-weight', 'font-size', 'line-height', 'font-family'));
     $this->combine_props($props, 'font', array('font-style', 'font-variant', 'font-size', 'line-height', 'font-family'));
     $this->combine_props($props, 'font', array('font-variant', 'font-weight', 'font-size', 'font-family'));
     $this->combine_props($props, 'font', array('font-style', 'font-weight', 'font-size', 'font-family'));
     $this->combine_props($props, 'font', array('font-style', 'font-variant', 'font-size', 'font-family'));
     $this->combine_props($props, 'font', array('font-variant', 'font-size', 'font-family'));
     $this->combine_props($props, 'font', array('font-weight', 'font-size', 'font-family'));
     $this->combine_props($props, 'font', array('font-style', 'font-size', 'font-family'));    
  }
 
  // this code is responsible for combining properties off rules
  // example: margin-left, margin-right, margin-top, margin-bottom can be combined to just margin:
  // the combined variable would be 'margin' and the parts would be the properties before
  function combine_props_borders(&$props, $combined, $parts) {
     // print_r($props);
 
     // split the properties and values
     for ($n = 0; $n < count($props); $n++)
     {
         // add the type to an array
         $props_type[] = substr($props[$n], 0, strpos($props[$n], ':'));
         // add the values to an array, although those are just stored and not processed
         $props_values[] = substr($props[$n], strpos($props[$n], ':')+1, strlen($props[$n]));
     }
     // assume it's combinable
     $combinable = TRUE;
     // loop through all the different properties that can be combined in this instance
     for ($n = 0; $n < count($parts); $n++)
     {
         // check if this property isn't contained within the combinable array
         if (!in_array($parts[$n], $props_type))
         {
             // if so, it can't be combined, so store that
             $combinable = FALSE;
         }
         else
         {
           
             $these_props = substr($props[$n], strpos($props[$n], ':')+1, 100);
         }
     }
     if ($combinable)
         // loop through all the different properties that can be combined in this instance
         for ($n = 0; $n < count($parts); $n++)
             // check if this property isn't contained within the combinable array
             if (in_array($parts[$n], $props_type))
                 if (substr($props[$n], strpos($props[$n], ':')+1, 100) <> $these_props)
                     // if so, it can't be combined, so store that
                     $combinable = FALSE;
 
     // if any of the properties were contained in the combinable array
     if ($combinable)
     {
         // loop through all the parts
         for ($a = 0; $a < count($parts); $a++)
         {
             // loop through all the properties found here
             for ($b = 0; $b < count($props_type); $b++)
             {
                 // check if it's the same
                 if ($props_type[$b] == $parts[$a])
                 {
                     // add the current values to the combined values
                     // this must be done in the correct order
                     $combined_values[] = $props_values[$b];
                     // no longer need the property since it's been added to the combined part, so remove it
                     $props[$b] = NULL;
                 }
             }
         }
         // add the new combined property with all the values of the individual propertys
         $props[] = $combined . ':' . $combined_values[0];
     }
  }
 
  // this code is responsible for combining properties off rules
  // example: margin-left, margin-right, margin-top, margin-bottom can be combined to just margin:
  // the combined variable would be 'margin' and the parts would be the properties before
  function combine_props(&$props, $combined, $parts) {
     // split the properties and values
     for ($n = 0; $n < count($props); $n++)
     {
         // add the type to an array
         $props_type[] = substr($props[$n], 0, strpos($props[$n], ':'));
         // add the values to an array, although those are just stored and not processed
         $props_values[] = substr($props[$n], strpos($props[$n], ':')+1, strlen($props[$n]));
     }
     // assume it's combinable
     $combinable = TRUE;
     // loop through all the different properties that can be combined in this instance
     for ($n = 0; $n < count($parts); $n++)
     {
         // check if this property isn't contained within the combinable array
         if (!in_array($parts[$n], $props_type))
             // if so, it can't be combined, so store that
             $combinable = FALSE;
     }
     // if any of the properties were contained in the combinable array
     if ($combinable)
     {
         // loop through all the parts
         for ($a = 0; $a < count($parts); $a++)
         {
             // loop through all the properties found here
             for ($b = 0; $b < count($props_type); $b++)
             {
                 // check if it's the same
                 if ($props_type[$b] == $parts[$a])
                 {
                     // add the current values to the combined values
                     // this must be done in the correct order
                     $combined_values[] = $props_values[$b];
                     // no longer need the property since it's been added to the combined part, so remove it
                     $props[$b] = NULL;
                 }
             }
         }
         // add the new combined property with all the values of the individual propertys
         $props[] = $combined . ':' . implode(' ', $combined_values);
     }
  }
 
  // this function just calls other ones
  function reduce_prop(&$item, $key) {
     // reduces six hex codes to three (#ff0000 -> #f00)
     $this->colours_to_hex($item);
     $this->short_hex($item);
     // removes useless values from padding and margins (margin: 4px 5px 4px 5px -> margin: 4px 5px)
     $this->compress_padding_and_margins($item);
  }
  
  function parseHEX($matches) {
    $hex = str_split(strtolower($matches[1]));
    if ($hex[0] == $hex[1] && $hex[2] == $hex[3] && $hex[4] == $hex[5]) {
      return '#' . $hex[0] . $hex[2] . $hex[4];
    }
    return strtolower($matches[0]);
  }
 
  // this code turns hex codes into short three-character hex codes when possible
  // for example:
  //  #ff0000 -> #f00
  //  #aabbcc -> #abc
  function short_hex(&$item) {
    $item = preg_replace_callback("/#([A-F|a-f|0-9]{6})/", array($this, 'parseHEX'), $item);
  }
 
  // this removes the useless values from padding and margin properties
  // this code is run after properties are combined (like margin-left, margin-top etc)
  // for example
  //  padding: 5px 5px 5px 5px -> padding: 5px
  //  margin: 2px 4px 2px 4px -> margin: 2px 4px
  //  padding: 3px 5px 9px 5px -> padding: 3px 5px 9px
  function compress_padding_and_margins(&$item) {
     // get the type and value of the property
     $item_parts = array();
     $pos = strpos($item, ':');
     $len = strlen($item);
     $item_parts[0] = substr($item, 0, $pos);
     $item_parts[1] = substr($item, ($pos + 1) - $len);
     // $item_parts = explode(':', $item);
 
     // check if this is a padding or margin property
     if ($item_parts[0] == 'padding' || $item_parts[0] == 'margin')
     {
         // place all the values into an array
         $values = explode(' ', $item_parts[1]);
       
         // switched based on the number of values found
         // no need to check if it's 1, because that can't be compressed
         switch (count($values))
         {
             // icey demonstrates the art of making pop corn:
             case 2:
                 // example: margin: 4px 4px
                 if ($values[0] == $values[1])
                     // example: margin: 4px
                     array_pop($values);
             break;            
             case 3:
                 // example: margin: 5px 7px 5px
                 if ($values[0] == $values[2])
                 {
                     // example: margin: 5px 7px
                     array_pop($values);
                     // example: margin: 4px 4px
                     if ($values[0] == $values[1])
                         // example: 4px
                         array_pop($values);
                 }    
             break;            
             case 4:
                 // example: margin: 3px 7px 9px 7px
                 if ($values[1] == $values[3])
                 {
                     // example: 3px 7px 9px
                     array_pop($values);
                     // example: 3px 4px 3px
                     if ($values[0] == $values[2])
                     {
                         // example: 3px 4px
                         array_pop($values);
                         // example: 7px 7px
                         if ($values[0] == $values[1])
                             // example: 7px
                             array_pop($values);
                     }                    
                 }    
             break;            
         }
         // check if any changes were made by comparing the original values to the current values
         if (implode(' ', $values) != $item_parts[1])
             // if so, change the item in the array to the shorter version
             $item = $item_parts[0] . ':' . implode(' ', $values);
     }
  }
 
  function remove_empty_rules() {
     // make these arrays available to this function
     
     
     // loop through each section of the css file and see if it contains no properties
     for ($a = 0; $a < count($this->file_selector); $a++)
     {
         // remove blank items from the array
         $this->file_props[$a] = array_values(array_diff($this->file_props[$a], array(NULL)));
         // check if this part has no properties
         if (!$this->file_props[$a][0])
         {
             // remove the empty prop part of the array and the class(es)
             array_splice($this->file_selector, $a, 1);
             array_splice($this->file_props, $a, 1);
             // decrease by one due to the decreased total
             $a--;
         }
     }
  }
 
  // now that the stylesheet has been compressed as much as possible,
  // the code to combine identical classes is used
  function combine_identical_rules() {
     // make these variables available locally
     // loop through each rule
     for ($a = 0; $a < count($this->file_props); $a++)
     {
         // loop from 0 up the current number, this ensures future processed properties aren't processed prematurely
         for ($b = 0; $b < $a; $b++)
         {
             if (substr($this->file_selector[$a][0], 0, 1) <> "@" && substr($this->file_selector[$b][0], 0, 1) <> "@")
             {
                 // check if this rule is identical to an earlier one
                 if (!array_diff($this->file_props[$a], $this->file_props[$b]) && !array_diff($this->file_props[$b], $this->file_props[$a]))
                 {
                     // combine the selectors
                     $this->file_selector[$a] = array_unique(array_merge($this->file_selector[$a], $this->file_selector[$b]));
                     // remove the old properties
                     $this->file_props[$b] = array(NULL);
                     // remove the old selectors
                     $this->file_selector[$b] = array(NULL);
                 }
             }
         }
     }
  }
  function create_output() {
     $css = '';
     for ($a = 0; $a < count($this->file_selector); $a++) {
       for ($b = 0; $b < count($this->file_selector[$a]); $b++) {
           $this->file_selector[$a][$b] = $this->file_selector[$a][$b];
       }
       for ($b = 0; $b < count($this->file_props[$a]); $b++) {
         $parts = array();
         $property = $this->file_props[$a][$b];
         $pos = strpos($property, ':');
         $len = strlen($property);
         $parts[0] = substr($property, 0, $pos);
         $parts[1] = substr($property, ($pos + 1) - $len);
         // $parts = explode(':', $this->file_props[$a][$b]);
         $this->file_props[$a][$b] = $parts[0] . ':' . $parts[1];        
       }
       $css .= implode(',', $this->file_selector[$a]) . "{";
       $css .= implode(";", $this->file_props[$a]) . ";}\n";
     }
     return $css;
  }
}
