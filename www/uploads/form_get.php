<?php 
    if (isset($_GET["msgToJulien"]) && isset($_GET["msgToElie"]))
    {
        echo 'Thank you for these messages : <br><br>';
        echo 'Message sent to Julien :  " ' . htmlspecialchars($_GET["msgToJulien"]) . ' "<br>';
        echo 'Message sent to Elie   :  " ' . htmlspecialchars($_GET["msgToElie"]) . ' "<br>';
    }
    else
    {
        echo 'I think you forgot to leave a message !';
    }
?>