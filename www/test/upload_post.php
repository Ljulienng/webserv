<?php 
    https://www.w3schools.com/php/php_file_upload.asp
    chdir("..");
    $target_dir = getcwd() . "/uploads";
    $target_file = $target_dir . "/" . basename($_FILES["file"]["name"]);
    $uploadOk = 1;
    $fileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));   
    
    if ($_FILES["file"]["size"] == 0)
    {
        echo "Your file cannot be upload by php.<br>";
        $uploadOk = 0;
    }

    // Check file size
    if ($_FILES["file"]["size"] > 100000)
    {
        echo "Your file is too large.<br>";
        $uploadOk = 0;
    }

    // Allow certain file formats
    // if($fileType != "txt" && $fileType != "cpp" && $fileType != "hpp")
    // {
    //   echo "Sorry, only .txt .cpp and .hpp files are allowed.<br>";
    //   $uploadOk = 0;
    // }

    // Check if $uploadOk is set to 0 by an error
    if ($uploadOk == 0)
    {
        echo "Sorry, your file was not uploaded";
    // if everything is ok, try to upload file
    } 
    else
    {
        if (move_uploaded_file($_FILES["file"]["tmp_name"], $target_file))
        {
            echo "The file \" ". htmlspecialchars( basename( $_FILES["file"]["name"])). " \" has been uploaded in \" $target_dir \"";
        } 
        else
        {
            echo "Sorry, there was an error uploading your file [error type " .$_FILES["file"]["error"] ."]";
        }
    }
?>