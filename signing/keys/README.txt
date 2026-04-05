This directory is intentionally empty in the public repository.

The production signing key is stored as a GitHub Actions secret:
  FIRMWARE_SIGNING_KEY  — PEM-encoded EC P-256 private key (imgtool format)

To generate a key pair for local development:

  pip install imgtool
  imgtool keygen -k signing/keys/dev-root-ec256.pem -t ecdsa-p256

The dev key must NEVER be committed for production use.
The CI workflow injects the production key via ${{ secrets.FIRMWARE_SIGNING_KEY }}.
