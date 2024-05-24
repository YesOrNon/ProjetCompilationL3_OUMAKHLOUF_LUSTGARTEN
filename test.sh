err=0
nbtest=0

exec > rep/rapport.txt 2>&1

echo
echo "Lancement des tests sur les fichiers valides:"
echo
for test in test/good/*
do
    ((nbtest++))
    echo "  $test"
    bin/tpcc < $test
    return=$?
    echo "  Valeur de retour: $return"
    if [[ $return -ne 0 ]]
    then
        echo "  Une erreur a été repéré dans le fichier $test"
        ((err+=1))
    fi
    echo
dones

ehco
echo "Lancement des tests sur les fichiers contenant une erreur syntaxique:"
echo
for test in test/syn-err/*
do
    ((nbtest++))
    echo "  $test"
    bin/tpcc < $test
    return=$?
    echo "  Valeur de retour: $return"
    if [[ $return -ne 1 ]]
    then
        echo "  Aucune erreur a été repéré dans le fichier $test"
        ((err++))
    fi
    echo
done

echo
echo "Lancement des tests sur les fichiers contentant une erreure sémantique:"
echo
for test in test/sem-err/*
do
    ((nbtest++))
    echo "  $test"
    bin/tpcc < $test
    return=$?
    echo "  Valeur de retour: $return"
    if [[ $return -ne 2 ]]
    then
        echo "  Aucune erreur a été repéré dans le fichier $test"
        ((err+=1))
    fi
    echo
done

echo
echo "Lancement des test sur les fichiers contant un Warning:"
echo
for test in test/warn/*
do
    ((nbtest++))
    echo "  $test"
    bin/tpcc < $test
    return=$?
    echo "  Valeur de retour: $return"
    if [[ $return -ne 0 ]]
    then
        echo "  Une erreur a été repéré dans le fichier $test"
        ((err+=1))
    fi
    echo
done

perc=$(((nbtest-err) * 100 /nbtest))
echo
echo "Pourcentage de réussite des tests: $perc"