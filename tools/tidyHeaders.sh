#!/usr/bin/php

<?php

/*
Copyright (C) 2015-2018 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

$UPDATE_FILES = false;

function funcSort($a, $b)
{
	$a = str_replace("*", "", $a);
	$b = str_replace("*", "", $b);
	
	$aParts = explode(" ", $a);
	$bParts = explode(" ", $b);
	
	return strcmp($aParts[2], $bParts[2]);
}

function updateExterns($header, $defines, $functions, $structs)
{
	asort($defines);
	usort($functions, "funcSort");
	asort($structs);
	
	$newHeader = [];
	$wasBlank = false;
	
	foreach ($header as $line)
	{
		if (strlen($line) > 1 || (strlen($line) == 1 && !$wasBlank))
		{
			$newHeader[] = $line;
		}
		
		$wasBlank = strlen($line) == 1;
	}

	if (count($defines) > 0)
	{
		$newHeader = array_merge($newHeader, $defines);
		$newHeader[] = "\n";
	}
	
	if (count($functions) > 0)
	{
		$newHeader = array_merge($newHeader, $functions);
		$newHeader[] = "\n";
	}

	if (count($structs) > 0)
	{
		$newHeader = array_merge($newHeader, $structs);
		$newHeader[] = "\n";
	}

	return $newHeader;
}

function cleanHeader($headerFile)
{
	global $UPDATE_FILES;

	$func_pattern = "/(([A-Z0-9_]+)\\()/i";
	$struct_pattern = "/([A-Z]+);/i";
	$define_pattern = "/#define ([A-Z]+)/i";

	$bodyFile = $headerFile;
	$bodyFile[strlen($bodyFile) - 1] = 'c';
	
	if (file_exists($bodyFile))
	{
		$header = file($headerFile);
		$body = file_get_contents($bodyFile);
		$lines = [];
		$defines = [];
		$functions = [];
		$structs = [];
	
		$i = 0;
		$hasChanges = false;
		
		foreach ($header as $line)
		{
			if (preg_match("/extern|define/", $line) && strstr($line, "getTranslatedString") === FALSE)
			{
				preg_match($func_pattern, $line, $matches);
				
				if (count($matches) == 3)
				{
					unset($header[$i]);
					
					$extern = $matches[2];
					
					if (!preg_match_all("/\b[(]?${extern}[\\(;,)\\n]/", $body))
					{
						if (!$hasChanges)
						{
							echo "$headerFile\n";
							$hasChanges = true;
						}
						echo "\t- $line";
					}
					
					if (!in_array($line, $lines))
					{
						$functions[] = $line;
					}
					else
					{
						if (!$hasChanges)
						{
							echo "$headerFile\n";
							$hasChanges = true;
						}
						echo "\t- $line";
					}
				}
				
				preg_match($struct_pattern, $line, $matches);

				if (count($matches) == 2)
				{
					unset($header[$i]);
					
					$extern = $matches[1];
					
					$externs[] = $extern;
					
					if (!preg_match_all("/\b${extern}([\\.\\-\\);]| =)/", $body))
					{
						if (!$hasChanges)
						{
							echo "$headerFile\n";
							$hasChanges = true;
						}
						echo "\t- $line";
					}
					
					if (!in_array($line, $lines))
					{
						$structs[] = $line;
					}
					else
					{
						if (!$hasChanges)
						{
							echo "$headerFile\n";
							$hasChanges = true;
						}
						echo "\t- $line";
					}
				}
				
				preg_match($define_pattern, $line, $matches);

				if (count($matches) == 2)
				{
					unset($header[$i]);
					
					$extern = $matches[1];
					
					$externs[] = $extern;
					
					if (strstr($body, "$extern") === FALSE)
					{
						if (!$hasChanges)
						{
							echo "$headerFile\n";
							$hasChanges = true;
						}
						echo "\t- $line";
					}
					
					if (!in_array($line, $lines))
					{
						$defines[] = $line;
					}
					else
					{
						if (!$hasChanges)
						{
							echo "$headerFile\n";
							$hasChanges = true;
						}
						echo "\t- $line";
					}
				}
			}
			
			$i++;
		}
		
		$header = updateExterns($header, $defines, $functions, $structs);
		
		if ($UPDATE_FILES)
		{
			file_put_contents($headerFile, $header);
		}
	}
}

function recurseDir($dir)
{
	if ($dir != "../src/json")
	{
		$files = array_diff(scandir($dir), array('..', '.'));
		
		foreach ($files as $file)
		{
			if (is_dir("$dir/$file"))
			{
				recurseDir("$dir/$file");
			}
			else if (strstr($file, ".h") !== FALSE)
			{
				cleanHeader("$dir/$file");
			}
		}
	}
}

if (isset($argv[1]))
{
	$UPDATE_FILES = ($argv[1] == "-commit");
}

recurseDir("../src");

if (!$UPDATE_FILES)
{
	echo "\nNo files updated. Use -commit to update headers\n";
}

?>
