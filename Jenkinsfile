pipeline {
    agent any
    triggers {
        GenericTrigger(causeString: 'Relay Push Service',  token: 'Graphics.Build')
    }
    options {

        buildDiscarder logRotator(
            artifactDaysToKeepStr: '', 
            artifactNumToKeepStr: '', 
            daysToKeepStr: '1', 
            numToKeepStr: '10')
    }

    stages {
       stage('Update Submodules') {
            steps {
                sh script: 'git submodule update --init --merge'
            }
        }
        stage('Build Project') {

            steps {
                sh script: 'cmake .'
                sh script: 'make clean'
                sh script: 'make'
            }
        }
        stage('Finalize') {
            steps {
                deleteDir()
            }
        }
    }
}